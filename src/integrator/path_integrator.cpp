#include <array>
#include "renderer/renderer.h"
#include "material/material.h"
#include "lights/light.h"
#include "lights/occlusion_tester.h"
#include "material/bsdf.h"
#include "integrator/path_integrator.h"

PathIntegrator::PathIntegrator(int min_depth, int max_depth) : min_depth(min_depth), max_depth(max_depth){}
Colorf PathIntegrator::illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &r,
	DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const
{
	//Allocate and generate samples for lights and bsdfs and path directions
	auto *l_samples_u = pool.alloc_array<std::array<float, 2>>(max_depth + 1);
	auto *l_samples_comp = pool.alloc_array<float>(max_depth + 1);
	auto *bsdf_samples_u = pool.alloc_array<std::array<float, 2>>(max_depth + 1);
	auto *bsdf_samples_comp = pool.alloc_array<float>(max_depth + 1);
	auto *path_samples_u = pool.alloc_array<std::array<float, 2>>(max_depth + 1);
	auto *path_samples_comp = pool.alloc_array<float>(max_depth + 1);
	sampler.get_samples(l_samples_u, max_depth + 1);
	sampler.get_samples(l_samples_comp, max_depth + 1);
	sampler.get_samples(bsdf_samples_u, max_depth + 1);
	sampler.get_samples(bsdf_samples_comp, max_depth + 1);
	sampler.get_samples(path_samples_u, max_depth + 1);
	sampler.get_samples(path_samples_comp, max_depth + 1);

	//Path throughput is the current product of bsdf values and geometry terms so far,
	//divided by their pdfs. illum is the current illumination along the path
	Colorf path_throughput, illum;
	//Current ray to find next path point
	RayDifferential ray{r};
	//If the last bounce was a specular one
	bool specular_bounce = false;
	//The current piece of geometry hit
	DifferentialGeometry *dg_current = &dg;
	//Next tracks the next geometry in the path
	DifferentialGeometry next;
	for (int bounce = 0; ; ++bounce){
		//Sample emissive objects on the first ray for directly visible ones or in the case of
		//specular bounces, as we don't compute them in estimate direct
		if (bounce == 0 || specular_bounce){
			const AreaLight *area_light = dg_current->node->get_area_light();
			if (area_light){
				illum += path_throughput * area_light->radiance(dg_current->point, dg_current->normal, -ray.d);
			}
		}
		if (!dg_current->node->get_material()){
			return illum;
		}
		//Get the hit point information
		dg_current->compute_differentials(ray);
		BSDF *bsdf = dg_current->node->get_material()->get_bsdf(*dg_current, pool);
		const Point &p = bsdf->dg.point;
		const Normal &n = bsdf->dg.normal;
		Vector w_o = -ray.d;

		//Uniformly sample one of the lights contribution to the point
		illum += path_throughput * uniform_sample_one_light(scene, renderer, p, n, w_o, *bsdf,
			LightSample{l_samples_u[bounce], l_samples_comp[bounce]},
			BSDFSample{bsdf_samples_u[bounce], bsdf_samples_comp[bounce]});

		//Determine our new path direction by sampling the BSDF
		Vector w_i;
		float pdf_val = 0;
		BxDFTYPE sampled_type;
		Colorf f = bsdf->sample(w_o, w_i, path_samples_u[bounce], path_samples_comp[bounce],
			pdf_val, BxDFTYPE::ALL, &sampled_type);
		//If there's no further illumination or probability for this path, we can stop
		if (f.is_black() || pdf_val == 0){
			break;
		}
		specular_bounce = (sampled_type & BxDFTYPE::SPECULAR) != 0;
		path_throughput *= f * std::abs(w_i.dot(n)) / pdf_val;
		ray = RayDifferential{p, w_i, ray, 0.001};

		//Check if we're at a point where we should start considering to terminate the path
		//or have hit max depth and need to stop
		if (bounce > min_depth){
			float cont_prob = std::min(0.5f, path_throughput.luminance());
			if (sampler.random_float() > cont_prob){
				break;
			}
			//Re-weight sum terms accordingly with Russian roulette weight
			path_throughput /= cont_prob;
		}
		if (bounce == max_depth){
			break;
		}

		//Find the next vertex on the path
		if (!scene.get_root().intersect(ray, next)){
			break;
		}
		dg_current = &next;
	}
	return illum;
}

