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
#include "monte_carlo/util.h"
#include "integrator/photon_map_integrator.h"

/*
 * Simpson's kernel for density estimation of photons
 */
float simpson_kernel(const Point &a, const Point &b, float max_dist_sqr){
	float s = (1 - a.distance_sqr(b) / max_dist_sqr);
	return 3 * INV_PI * s * s;
}

bool PhotonMapIntegrator::NearPhoton::operator<(const PhotonMapIntegrator::NearPhoton &b) const {
	//Arbitrarily break times the same way the KdPointTree does
	return dist_sqr == b.dist_sqr ? photon < b.photon : dist_sqr < b.dist_sqr;
}

PhotonMapIntegrator::ShootingTask::ShootingTask(PhotonMapIntegrator &integrator, const Scene &scene,
	const Distribution1D &light_distrib, int seed)
	: integrator(integrator), scene(scene), light_distrib(light_distrib),
	sampler(std::make_unique<LDSampler>(0, 1, 0, 1, 2, seed))
{}
void PhotonMapIntegrator::ShootingTask::shoot(){
	MemoryPool pool;
	bool caustic_done = integrator.num_caustic.load(std::memory_order_consume) == integrator.num_caustic_wanted;
	bool indirect_done = integrator.num_indirect.load(std::memory_order_consume) == integrator.num_indirect_wanted;
	//Trace batches of 2048 photons then check if we've reached the number of desired photons of each type
	const static int batch_size = 2048;
	while (true){
		for (int i = 0; i < batch_size; ++i){
			std::array<float, 6> u;
			std::generate(u.begin(), u.end(), std::bind(&Sampler::random_float, sampler.get()));
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
		integrator.num_direct.fetch_add(batch_size, std::memory_order_acq_rel);
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
			if (deposited && sampler.random_float() < 0.50){
				//Make sure the normal of the surface faces the right direction when we save it (eg. in case of transmission)
				Normal n = w_o.dot(dg.normal) < 0 ? -dg.normal : dg.normal;
				radiance_photons.push_back(RadiancePhoton{dg.point, n, Colorf{0}});
				//Also store the reflectance and transmittance at the point so we can compute
				//the radiance after mapping all photons
				radiance_reflectance.emplace_back(bsdf->rho_hh(sampler, pool, BxDFTYPE::ALL_REFLECTION));
				radiance_transmittance.emplace_back(bsdf->rho_hh(sampler, pool, BxDFTYPE::ALL_TRANSMISSION));
			}
		}
		if (photon_depth > integrator.max_phot_depth){
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

PhotonMapIntegrator::RadianceTask::RadianceTask(const PhotonMapIntegrator &integrator, int begin, int end,
	std::vector<RadiancePhoton> &radiance_photons, const std::vector<Colorf> &radiance_reflectance,
	const std::vector<Colorf> &radiance_transmittance)
	: integrator(integrator), begin(begin), end(end), radiance_photons(radiance_photons),
	radiance_reflectance(radiance_reflectance), radiance_transmittance(radiance_transmittance)
{}
void PhotonMapIntegrator::RadianceTask::compute(){
	int caustic_paths = integrator.num_caustic.load(std::memory_order_consume);
	int indirect_paths = integrator.num_indirect.load(std::memory_order_consume);
	int direct_paths = integrator.num_direct.load(std::memory_order_consume);
	std::vector<NearPhoton> near_photons(integrator.query_size);
	for (int i = begin; i < end; ++i){
		auto &p = radiance_photons[i];
		const auto &refl = radiance_reflectance[i];
		const auto &trans = radiance_reflectance[i];
		//We compute the radiance photon using estimates of the reflectance and transmittance
		//from the irradiance of photons in the maps
		if (!refl.is_black()){
			Colorf irrad;
			if (integrator.caustic_map != nullptr){
				irrad += photon_irradiance(*integrator.caustic_map, caustic_paths, integrator.query_size,
					near_photons.data(), integrator.max_dist_sqr, p.position, p.normal);
			}
			if (integrator.indirect_map != nullptr){
				irrad += photon_irradiance(*integrator.indirect_map, indirect_paths, integrator.query_size,
					near_photons.data(), integrator.max_dist_sqr, p.position, p.normal);
			}
			if (integrator.direct_map != nullptr){
				irrad += photon_irradiance(*integrator.direct_map, direct_paths, integrator.query_size,
					near_photons.data(), integrator.max_dist_sqr, p.position, p.normal);
			}
			p.emit += INV_PI * refl * irrad;
		}
		if (!trans.is_black()){
			Colorf irrad;
			if (integrator.caustic_map != nullptr){
				irrad += photon_irradiance(*integrator.caustic_map, caustic_paths, integrator.query_size,
					near_photons.data(), integrator.max_dist_sqr, p.position, -p.normal);
			}
			if (integrator.indirect_map != nullptr){
				irrad += photon_irradiance(*integrator.indirect_map, indirect_paths, integrator.query_size,
					near_photons.data(), integrator.max_dist_sqr, p.position, -p.normal);
			}
			if (integrator.direct_map != nullptr){
				irrad += photon_irradiance(*integrator.direct_map, direct_paths, integrator.query_size,
					near_photons.data(), integrator.max_dist_sqr, p.position, -p.normal);
			}
			p.emit += INV_PI * trans * irrad;
		}
	}
}

void PhotonMapIntegrator::PhotonQueryCallback::operator()(const Point&, const Photon &photon, float dist_sqr,
	float &max_dist_sqr)
{
	if (found < query_size){
		queried_photons[found++] = NearPhoton{&photon, dist_sqr};
		//If we've hit our query size start shrinking the search radius so we only get photons
		//closer than our farthest one
		if (found == query_size){
			std::make_heap(queried_photons, queried_photons + query_size);
			max_dist_sqr = queried_photons[0].dist_sqr;
		}
	}
	//Any photons found after filling our desired number must be closer than the furthest one we found
	//so replace it and shrink the search distance
	else {
		std::pop_heap(queried_photons, queried_photons + query_size);
		queried_photons[query_size - 1] = NearPhoton{&photon, dist_sqr};
		std::push_heap(queried_photons, queried_photons + query_size);
		max_dist_sqr = queried_photons[0].dist_sqr;
	}
}

void PhotonMapIntegrator::RadianceQueryCallback::operator()(const Point&, const RadiancePhoton &p, float dist_sqr,
	float &max_dist_sqr)
{
	//Since we narrow the search radius any photon we find will be closer than the last
	if (normal.dot(p.normal) > 0){
		photon = &p;
		max_dist_sqr = dist_sqr;
	}
}

PhotonMapIntegrator::PhotonMapIntegrator(int num_caustic_wanted, int num_indirect_wanted, int max_depth,
	int max_phot_depth, int query_size, int final_gather_samples, float max_dist_sqr, float gather_angle)
	: num_caustic_wanted(num_caustic_wanted), num_indirect_wanted(num_indirect_wanted), max_depth(max_depth),
	max_phot_depth(max_phot_depth), query_size(query_size), final_gather_samples(final_gather_samples),
	max_dist_sqr(max_dist_sqr), gather_angle(gather_angle), num_caustic(0), num_indirect(0), num_direct(0)
{}
void PhotonMapIntegrator::preprocess(const Scene &scene){
	if (scene.get_light_cache().empty()){
		return;
	}
	auto start = std::chrono::high_resolution_clock::now();
	std::vector<Photon> caustic_photons, indirect_photons, direct_photons;
	std::vector<RadiancePhoton> radiance_photons;
	std::vector<Colorf> radiance_reflectance, radiance_transmittance;
	std::cout << "PhotonMapIntegrator: shooting photons" << std::endl;
	shoot_photons(caustic_photons, indirect_photons, direct_photons, radiance_photons,
		radiance_reflectance, radiance_transmittance, scene);
	std::cout << "PhotonMapIntegrator: building photon maps" << std::endl;

	std::vector<std::thread> threads;
	threads.reserve(3);
	if (!caustic_photons.empty()){
		threads.emplace_back([this, &caustic_photons](){
			caustic_map = std::make_unique<KdPointTree<Photon>>(std::move(caustic_photons));
		});
	}
	if (!indirect_photons.empty()){
		threads.emplace_back([this, &indirect_photons](){
			indirect_map = std::make_unique<KdPointTree<Photon>>(std::move(indirect_photons));
		});
	}
	if (!direct_photons.empty()){
		threads.emplace_back([this, &direct_photons](){
			direct_map = std::make_unique<KdPointTree<Photon>>(std::move(direct_photons));
		});
	}
	for (auto &t : threads){
		t.join();
	}

	//Compute radiance photon emittances now that we've got the photon maps built
	if (!radiance_photons.empty() && final_gather_samples > 0){
		std::cout << "PhotonMapIntegrator: computing radiance photon emittance" << std::endl;
		//We use the number of hw threads to compute radiance + 1 for any overflow photons
		int hw_threads = std::thread::hardware_concurrency();
		int phot_per_task = radiance_photons.size() / hw_threads;
		int num_tasks = radiance_photons.size() % hw_threads == 0 ? hw_threads : hw_threads + 1;
		std::vector<RadianceTask> tasks;
		threads.clear();
		threads.reserve(num_tasks);
		tasks.reserve(num_tasks);
		for (int i = 0; i < num_tasks; ++i){
			int begin = i * phot_per_task;
			int end = (i + 1) * phot_per_task;
			end = end < radiance_photons.size() ? end : radiance_photons.size();
			tasks.emplace_back(*this, begin, end, radiance_photons, radiance_reflectance, radiance_transmittance);
			threads.emplace_back(&RadianceTask::compute, &tasks.back());
		}
		//Wait for all radiance computation tasks to complete
		for (auto &t : threads){
			t.join();
		}
		radiance_map = std::make_unique<KdPointTree<RadiancePhoton>>(std::move(radiance_photons));
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = end - start;
	std::cout << "PhotonMapIntegrator: building photon maps took: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
		<< "ms\n";
	//Store the total number of paths in non-atomic vars so we can access them quickly
	caustic_paths = num_caustic.load(std::memory_order_consume);
	indirect_paths = num_indirect.load(std::memory_order_consume);
	//We're done with the direct map now
	direct_map = nullptr;
}
Colorf PhotonMapIntegrator::illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
	DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const
{
	Colorf illum;
	Vector w_o = -ray.d;
	const AreaLight *area_light = dg.node->get_area_light();
	if (area_light){
		illum += area_light->radiance(dg.point, dg.normal, w_o);
	}
	if (dg.node->get_material() == nullptr){
		return illum;
	}
	BSDF *bsdf = dg.node->get_material()->get_bsdf(dg, pool);
	const Point &p = bsdf->dg.point;
	const Normal &n = bsdf->dg.normal;
	auto *near_photons = pool.alloc_array<NearPhoton>(query_size);

	illum += uniform_sample_all_lights(scene, renderer, p, n, w_o, *bsdf, sampler, pool);
	if (caustic_map != nullptr){
		illum += photon_radiance(*caustic_map, caustic_paths, query_size, near_photons, max_dist_sqr,
			*bsdf, sampler, pool, dg, w_o);
	}
	const static BxDFTYPE NON_SPECULAR_BXDF = BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::TRANSMISSION
		| BxDFTYPE::DIFFUSE | BxDFTYPE::GLOSSY);
	if (indirect_map != nullptr && bsdf->num_bxdfs(NON_SPECULAR_BXDF) > 0){
		if (radiance_map != nullptr){
			illum += final_gather(scene, renderer, ray, p, n, *bsdf, sampler, pool);
		}
		else {
			illum += photon_radiance(*indirect_map, indirect_paths, query_size, near_photons, max_dist_sqr,
				*bsdf, sampler, pool, dg, w_o);
		}
	}
	//We handle specular reflection and transmission through standard recursive ray tracing
	const static BxDFTYPE SPECULAR_BXDF = BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::TRANSMISSION | BxDFTYPE::SPECULAR);
	if (ray.depth < max_depth && bsdf->num_bxdfs(SPECULAR_BXDF) > 0){
		illum += spec_reflect(ray, *bsdf, renderer, scene, sampler, pool);
		illum += spec_transmit(ray, *bsdf, renderer, scene, sampler, pool);
	}
	return illum;
}
Colorf PhotonMapIntegrator::final_gather(const Scene &scene, const Renderer&, const RayDifferential &ray,
	const Point &p, const Normal &n, const BSDF &bsdf, Sampler &sampler, MemoryPool &pool) const
{
	Vector w_o = -ray.d;
	//Query nearby indirect photons to get an estimate of the average direction of incident illumination at the point
	PhotonQueryCallback phot_query{pool.alloc_array<NearPhoton>(query_size), query_size, 0};
	//Re run the query until we get the desired number of photons or go over a limit in how big we're letting the query get
	for (float query_dist_sqr = max_dist_sqr; phot_query.found < query_size && query_dist_sqr < 8 * max_dist_sqr; query_dist_sqr *= 2){
		phot_query.found = 0;
		float dist = query_dist_sqr;
		indirect_map->query(p, dist, phot_query);
	}
	auto *indirect_dirs = pool.alloc_array<Vector>(phot_query.found);
	std::transform(phot_query.queried_photons, phot_query.queried_photons + phot_query.found, indirect_dirs,
		[](const auto &p){ return p.photon->w_i; });

	//Use the BSDF to perform final gathering
	auto *bsdf_samples_u = pool.alloc_array<std::array<float, 2>>(final_gather_samples);
	auto *bsdf_samples_comp = pool.alloc_array<float>(final_gather_samples);
	sampler.get_samples(bsdf_samples_u, final_gather_samples);
	sampler.get_samples(bsdf_samples_comp, final_gather_samples);
	Colorf gathered;
	for (int i = 0; i < final_gather_samples; ++i){
		//Sample direction from the BSDF for a gather ray
		Vector w_i;
		float pdf_val = 0;
		Colorf f = bsdf.sample(w_o, w_i, bsdf_samples_u[i], bsdf_samples_comp[i], pdf_val,
			BxDFTYPE(BxDFTYPE::ALL & ~BxDFTYPE::SPECULAR));
		if (f.is_black() || pdf_val == 0){
			continue;
		}
		//Trace the gather ray in the scene and use the nearest radiance photon at the hit point
		//to estimate illumination
		RayDifferential gather_ray{p, w_i, ray, 0.001};
		DifferentialGeometry dg;
		if (scene.get_root().intersect(gather_ray, dg)){
			Normal n_gather = dg.normal.dot(-gather_ray.d) < 0 ? -dg.normal : dg.normal;
			RadianceQueryCallback rad_query{n_gather, nullptr}; 
			float query_dist = std::numeric_limits<float>::infinity();
			radiance_map->query(dg.point, query_dist, rad_query);
			Colorf emit = rad_query.photon != nullptr ? rad_query.photon->emit : Colorf{0};

			//Compute overall pdf of sampling w_i direction from the photon distribution using a gather angle sized cone
			float photon_pdf = 0;
			float cone_pdf = uniform_cone_pdf(gather_angle);
			for (int j = 0; j < phot_query.found; ++j){
				if (indirect_dirs[j].dot(w_i) > 0.999 * gather_angle){
					photon_pdf += cone_pdf;
				}
			}
			photon_pdf /= phot_query.found;
			float weight = power_heuristic(final_gather_samples, pdf_val, final_gather_samples, photon_pdf);
			gathered += f * emit * std::abs(w_i.dot(n)) * weight / pdf_val;
		}
	}
	Colorf illum = gathered / final_gather_samples;

	//Now use nearby photons to do final gathering
	gathered = Colorf{0};
	sampler.get_samples(bsdf_samples_u, final_gather_samples, final_gather_samples);
	sampler.get_samples(bsdf_samples_comp, final_gather_samples, final_gather_samples);
	for (int i = 0; i < final_gather_samples; ++i){
		//Uniformly select a photon and use it to sample a vector along the cone centered about its incident direction
		int photon_id = std::min(phot_query.found - 1,
			static_cast<int>(bsdf_samples_comp[i] * phot_query.found));
		Vector w_x, w_y;
		coordinate_system(indirect_dirs[photon_id], w_x, w_y);
		Vector w_i = uniform_sample_cone(bsdf_samples_u[i], gather_angle, w_x, w_y, indirect_dirs[photon_id]);

		Colorf f = bsdf(w_o, w_i);
		if (f.is_black()){
			continue;
		}
		//Trace the gather ray in the scene and use the nearest radiance photon at the hit point
		//to estimate illumination
		RayDifferential gather_ray{p, w_i, ray, 0.001};
		DifferentialGeometry dg;
		if (scene.get_root().intersect(gather_ray, dg)){
			Normal n_gather = dg.normal.dot(-gather_ray.d) < 0 ? -dg.normal : dg.normal;
			RadianceQueryCallback rad_query{n_gather, nullptr}; 
			float query_dist = std::numeric_limits<float>::infinity();
			radiance_map->query(dg.point, query_dist, rad_query);
			Colorf emit = rad_query.photon != nullptr ? rad_query.photon->emit : Colorf{0};

			//Compute overall pdf of sampling w_i direction from the photon distribution using a gather angle sized cone
			float photon_pdf = 0;
			float cone_pdf = uniform_cone_pdf(gather_angle);
			for (int j = 0; j < phot_query.found; ++j){
				if (indirect_dirs[j].dot(w_i) > 0.999 * gather_angle){
					photon_pdf += cone_pdf;
				}
			}
			photon_pdf /= phot_query.found;
			float pdf_val = bsdf.pdf(w_o, w_i);
			float weight = power_heuristic(final_gather_samples, pdf_val, final_gather_samples, photon_pdf);
			gathered += f * emit * std::abs(w_i.dot(n)) * weight / photon_pdf;
		}
	}
	return illum + gathered / final_gather_samples;
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
	std::uniform_int_distribution<int> seed;
	for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i){
		shooting_tasks.emplace_back(*this, scene, light_distrib, seed(rng));
		threads.emplace_back(&ShootingTask::shoot, &shooting_tasks.back());
	}
	//Wait for all shooting tasks to complete, collecting results from each task
	for (auto &t : threads){
		t.join();
	}
	while (!shooting_tasks.empty()){
		auto &task = shooting_tasks.back();
		std::copy(task.caustic_photons.begin(), task.caustic_photons.end(), std::back_inserter(caustic_photons));
		std::copy(task.indirect_photons.begin(), task.indirect_photons.end(), std::back_inserter(indirect_photons));
		std::copy(task.direct_photons.begin(), task.direct_photons.end(), std::back_inserter(direct_photons));
		std::copy(task.radiance_photons.begin(), task.radiance_photons.end(), std::back_inserter(radiance_photons));
		std::copy(task.radiance_reflectance.begin(), task.radiance_reflectance.end(),
			std::back_inserter(radiance_reflectance));
		std::copy(task.radiance_transmittance.begin(), task.radiance_transmittance.end(),
			std::back_inserter(radiance_transmittance));
		shooting_tasks.pop_back();
	}
}
Colorf PhotonMapIntegrator::photon_irradiance(const KdPointTree<Photon> &photons, int num_paths, int query_size,
	NearPhoton *near_photons, float max_dist_sqr, const Point &p, const Normal &n)
{
	PhotonQueryCallback callback{near_photons, query_size, 0};
	photons.query(p, max_dist_sqr, callback);
	if (callback.found == 0){
		return Colorf{0};
	}
	Colorf irrad;
	//Compute average of the photons found that are on our side of the hemisphere
	for (int i = 0; i < callback.found; ++i){
		if (near_photons[i].photon->w_i.dot(n) > 0){
			irrad += near_photons[i].photon->weight;
		}
	}
	return irrad / (num_paths * max_dist_sqr * PI);
}
Colorf PhotonMapIntegrator::photon_radiance(const KdPointTree<Photon> &photons, int num_paths, int query_size,
	NearPhoton *near_photons, float max_dist_sqr, BSDF &bsdf, Sampler &sampler, MemoryPool &pool,
	const DifferentialGeometry &dg, const Vector &w_o)
{
	Colorf rad;
	const static BxDFTYPE NON_SPECULAR_BXDF = BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::TRANSMISSION
		| BxDFTYPE::DIFFUSE | BxDFTYPE::GLOSSY);
	if (bsdf.num_bxdfs(NON_SPECULAR_BXDF) > 0){
		//Find the photons at the intersection point that we'll use to estimate radiance
		PhotonQueryCallback callback{near_photons, query_size, 0};
		photons.query(dg.point, max_dist_sqr, callback);
		if (callback.found == 0){
			return rad;
		}
		Normal n_fwd = w_o.dot(bsdf.dg.normal) < 0 ? -bsdf.dg.normal : bsdf.dg.normal;
		//Handle glossy and diffuse separately since we can be more efficient for pure diffuse since
		//their bsdf is constant, as described in PBR
		if (bsdf.num_bxdfs(BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::TRANSMISSION | BxDFTYPE::GLOSSY)) > 0){
			//Compute average radiance of photons w/ density estimate
			for (int i = 0; i < callback.found; ++i){
				const Photon &p = *near_photons[i].photon;
				float k = simpson_kernel(p.position, dg.point, max_dist_sqr);
				rad += k / (num_paths * max_dist_sqr) * bsdf(w_o, p.w_i) * p.weight;
			}
		}
		else {
			Colorf refl, trans;
			for (int i = 0; i < callback.found; ++i){
				const Photon &p = *near_photons[i].photon;
				float k = simpson_kernel(p.position, dg.point, max_dist_sqr);
				if (p.w_i.dot(n_fwd) > 0){
					refl += k / (num_paths * max_dist_sqr) * p.weight;
				}
				else {
					trans += k / (num_paths * max_dist_sqr) * p.weight;
				}
			}
			rad += refl * bsdf.rho_hd(w_o, sampler, pool, BxDFTYPE::ALL_REFLECTION) * INV_PI
				+ trans * bsdf.rho_hd(w_o, sampler, pool, BxDFTYPE::ALL_TRANSMISSION) * INV_PI;
		}
	}
	return rad;
}

