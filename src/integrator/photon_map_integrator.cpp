#include <cmath>
#include <chrono>
#include <random>
#include <array>
#include <thread>
#include "scene.h"
#include "linalg/ray.h"
#include "geometry/differential_geometry.h"
#include "memory_pool.h"
#include "samplers/ld_sampler.h"
#include "lights/light.h"
#include "material/bsdf.h"
#include "integrator/photon_map_integrator.h"

PhotonMapIntegrator::ShootingTask::ShootingTask(PhotonMapIntegrator &integrator, const Scene &scene,
	const Distribution1D &light_distrib, float seed)
	: integrator(integrator), scene(scene), light_distrib(light_distrib),
	sampler(std::make_unique<LDSampler>(0, 1, 0, 1, 1, seed))
{}
void PhotonMapIntegrator::ShootingTask::shoot(){
	MemoryPool pool;
	int paths_traced = 0;
	bool caustic_done = integrator.num_caustic.load(std::memory_order_consume) == integrator.num_caustic_wanted;
	bool indirect_done = integrator.num_indirect.load(std::memory_order_consume) == integrator.num_indirect_wanted;
	//Trace batches of 2048 photons then check if we've reached the number of desired photons of each type
	while (true){
		const int batch_size = 2048;
		for (int i = 0; i < batch_size; ++i){
			std::array<float, 6> u;
			sampler->get_samples(u.data(), 6, paths_traced);
			//Choose a light to sample from based on the light CDF for the scene
			float light_pdf = 0;
			int light_num = light_distrib.sample_discrete(u[0], &light_pdf);
			//The unordered map isn't a random access container, so 'find' the light_num light
			auto lit = std::find_if(scene.get_light_cache().begin(), scene.get_light_cache().end(),
				[&light_num](const auto&){
					return light_num-- == 0;
				});
			//Now we can get an outgoing photon direction from the light
			const Light &light = *lit->second;
			RayDifferential ray;
			Normal n_l;
			float pdf_val = 0;
			Colorf emitted = light.sample(scene, LightSample{{u[1], u[2]}, u[3]}, {u[4], u[5]}, ray, n_l, pdf_val);
			if (pdf_val == 0 || emitted.is_black()){
				continue;
			}
			Colorf weight = std::abs(ray.d.dot(n_l)) * emitted / (pdf_val * light_pdf);
			if (weight.is_black()){
				continue;
			}
			//We've sampled a photon with some actual contribution leaving the light so trace it through the scene
			trace_photon(ray, weight, caustic_done, indirect_done, *sampler, pool);
			pool.free_blocks();
		}
		int num_caustic = integrator.num_caustic.fetch_add(batch_size, std::memory_order_acq_rel) + batch_size;
		int num_indirect = integrator.num_indirect.fetch_add(batch_size, std::memory_order_acq_rel) + batch_size;
		caustic_done = num_caustic >= integrator.num_caustic_wanted;
		indirect_done = num_indirect >= integrator.num_indirect_wanted;
		if (caustic_done && indirect_done){
			return;
		}
	}
}
void PhotonMapIntegrator::ShootingTask::trace_photon(const RayDifferential &r, Colorf weight, bool caustic_done,
	bool indirect_done, Sampler &sampler, MemoryPool &pool)
{
	//If the path is entirely specular then this is a caustic photon, true initially as it's ignored by direct photon
	bool specular_path = true;
	int photon_depth = 0;
	RayDifferential ray = r;
	DifferentialGeometry dg;
	while (scene.get_root().intersect(ray, dg)){
		++photon_depth;
		if (!dg.node->get_material()){
			break;
		}
		BSDF *bsdf = dg.node->get_material()->get_bsdf(dg, pool);
		//Check if this BSDF has non-specular components
		const static BxDFTYPE SPECULAR_BXDF = BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::TRANSMISSION | BxDFTYPE::SPECULAR);
		Vector w_o = -ray.d;
		//If the surface has non-specular components we can deposit the photon on the surface
		if (bsdf->num_bxdfs() > bsdf->num_bxdfs(SPECULAR_BXDF)){
			Photon photon{dg.point, weight, w_o};
			bool deposited = false;
			//If it's a specular path and not a direct photon deposit a caustic
			if (specular_path && photon_depth > 1){
				if (!caustic_done){
					caustic_photons.push_back(photon);
					deposited = true;
				}
			}
			//We also stop depositing direct photons when we finish indirect since we'd likely run out of memory otherwise
			else if (!indirect_done){
				if (photon_depth == 1){
					direct_photons.push_back(photon);
				}
				else {
					indirect_photons.push_back(photon);
				}
				deposited = true;
			}
			//Randomly create radiance photons with some low probability, using the same value here as PBR
			if (deposited && sampler.random_float() < 0.125){
				//Make sure the normal of the surface faces the right direction when we save it (eg. in case of transmission)
				Normal n = w_o.dot(dg.normal) < 0 ? -dg.normal : dg.normal;
				radiance_photons.push_back(RadiancePhoton{dg.point, n, Colorf{0}});
				//Also store the reflectance and transmittance at the point so we can compute
				//the radiance after mapping all photons
				radiance_reflectance.emplace_back(bsdf->rho_hh(sampler, pool, BxDFTYPE::ALL_REFLECTION));
				radiance_transmittance.emplace_back(bsdf->rho_hh(sampler, pool, BxDFTYPE::ALL_TRANSMISSION));
			}
		}
		if (photon_depth > integrator.max_depth){
			break;
		}
		//Sample an outgoing direction from the BSDF to continue tracing the photon in and weights and path info
		std::array<float, 2> u;
		float comp;
		sampler.get_samples(&u, 1, photon_depth);
		sampler.get_samples(&comp, 1, photon_depth);
		Vector w_i;
		float pdf_val = 0;
		BxDFTYPE sampled_type;
		Colorf f = bsdf->sample(w_o, w_i, u, comp, pdf_val, BxDFTYPE::ALL, &sampled_type);
		if (pdf_val == 0 || f.is_black()){
			break;
		}
		//Update weight and try to terminate photons with Russian Roulette based on how much the weight decreased
		//at this current intersection. Eg. a surface that absorbs many photons will reduce the weight a lot here
		//and increase the likelyhood of terminating photons. See PBR for deeper explanation
		Colorf weight_new = weight * f * std::abs(w_i.dot(bsdf->dg.normal)) / pdf_val;
		float cont_prob = std::min(1.f, weight_new.luminance() / weight.luminance());
		if (sampler.random_float() > cont_prob){
			break;
		}
		//If we do continue then do so with the luminance the same as it was before we scatted
		weight = weight_new / cont_prob;

		specular_path &= (sampled_type & BxDFTYPE::SPECULAR) != 0;
		//If we're done tracing indirect and this isn't a caustic photon there's no reason to continue
		if (indirect_done && !specular_path){
			break;
		}
		ray = RayDifferential{dg.point, w_i, ray, 0.001};
	}
}

PhotonMapIntegrator::PhotonMapIntegrator(int num_caustic_wanted, int num_indirect_wanted, int max_depth)
	: num_caustic_wanted(num_caustic_wanted), num_indirect_wanted(num_indirect_wanted), max_depth(max_depth),
	num_caustic(0), num_indirect(0)
{}
void PhotonMapIntegrator::preprocess(const Scene &scene){
	if (scene.get_light_cache().empty()){
		return;
	}
	std::vector<Photon> caustic_photons, indirect_photons, direct_photons;
	std::vector<RadiancePhoton> radiance_photons;
	std::vector<Colorf> radiance_reflectance, radiance_transmittance;
	shoot_photons(caustic_photons, indirect_photons, direct_photons, radiance_photons,
		radiance_reflectance, radiance_transmittance, scene);
	//Build our photon maps from the traced photons
	if (!caustic_photons.empty()){
		caustic_map = std::make_unique<KdPointTree<Photon>>(caustic_photons);
	}
	if (!indirect_photons.empty()){
		indirect_map = std::make_unique<KdPointTree<Photon>>(indirect_photons);
	}
	if (!direct_photons.empty()){
		direct_map = std::make_unique<KdPointTree<Photon>>(direct_photons);
	}
	//Compute radiance photon emittances now that we've got the photon maps built
	if (!radiance_photons.empty()){
		//TODO: launch tasks
		radiance_map = std::make_unique<KdPointTree<RadiancePhoton>>(radiance_photons);
	}
}
Colorf PhotonMapIntegrator::illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
	DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const
{
	return Colorf{0};
}
void PhotonMapIntegrator::shoot_photons(std::vector<Photon> &caustic_photons, std::vector<Photon> &indirect_photons,
	std::vector<Photon> &direct_photons, std::vector<RadiancePhoton> &radiance_photons,
	std::vector<Colorf> &radiance_reflectance, std::vector<Colorf> &radiance_transmittance, const Scene &scene)
{
	Distribution1D light_distrib = light_sampling_cdf(scene);
	//Allocate and launch the photon shooting tasks
	std::vector<std::thread> threads;
	std::vector<ShootingTask> shooting_tasks;
	shooting_tasks.reserve(std::thread::hardware_concurrency());
	threads.reserve(std::thread::hardware_concurrency());
	std::minstd_rand rng (std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()).count());
	std::uniform_real_distribution<float> distrib;
	for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i){
		shooting_tasks.emplace_back(*this, scene, light_distrib, distrib(rng));
		threads.emplace_back(&ShootingTask::shoot, &shooting_tasks.back());
	}
	//Wait for all shooting tasks to complete, collecting results from each task
	for (size_t i = 0; i < shooting_tasks.size(); ++i){
		threads[i].join();
		auto &task = shooting_tasks[i];
		std::copy(task.caustic_photons.begin(), task.caustic_photons.end(), std::back_inserter(caustic_photons));
		std::copy(task.indirect_photons.begin(), task.indirect_photons.end(), std::back_inserter(indirect_photons));
		std::copy(task.direct_photons.begin(), task.direct_photons.end(), std::back_inserter(direct_photons));
		std::copy(task.radiance_photons.begin(), task.radiance_photons.end(), std::back_inserter(radiance_photons));
		std::copy(task.radiance_reflectance.begin(), task.radiance_reflectance.end(),
			std::back_inserter(radiance_reflectance));
		std::copy(task.radiance_transmittance.begin(), task.radiance_transmittance.end(),
			std::back_inserter(radiance_transmittance));
	}
}

