#include <array>
#include "material/material.h"
#include "lights/light.h"
#include "lights/occlusion_tester.h"
#include "material/bsdf.h"
#include "renderer/renderer.h"
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
	Colorf path_throughput{1}, illum;
	//Current ray to find next path point
	RayDifferential ray{r};
	//If the last bounce was a specular one
	bool specular_bounce = false;
	//Next tracks the next geometry in the path
	DifferentialGeometry next = dg;
	//The current piece of geometry hit
	DifferentialGeometry *dg_current = &next;
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

/*
{
	std::cout << "WhittedIntegrator Node has area light? " << (dg.node->get_area_light() ? "true" : "false") << std::endl;
	return 0;
	Vector w_o = -ray.d;
	Colorf illum;
	const Material *mat = dg.node->get_material();
	const AreaLight *area_light = dg.node->get_area_light();
	if (area_light){
		illum += area_light->radiance(dg.point, dg.normal, w_o);
	}
	if (!mat){
		return illum;
	}
	dg.compute_differentials(ray);
	BSDF *bsdf = mat->get_bsdf(dg, pool);

	//Compute the incident light from all lights in the scene
	for (const auto &l : scene.get_light_cache()){
		int n_samples = l.second->n_samples;
		auto l_samples = pool.alloc_array<std::array<float, 2>>(n_samples);
		sampler.get_samples(l_samples, n_samples);
		for (int i = 0; i < n_samples; ++i){
			Vector w_i;
			float pdf_val = 0;
			OcclusionTester occlusion;
			Colorf li = l.second->sample(bsdf->dg.point, l_samples[i], w_i, pdf_val, occlusion);
			//If there's no light or no probability for this sample there's no illumination
			if (li.luminance() == 0 || pdf_val == 0){
				continue;
			}
			Colorf c = (*bsdf)(w_o, w_i);
			if (!c.is_black() && !occlusion.occluded(scene)){
				illum += c * li * std::abs(w_i.dot(bsdf->dg.normal)) / pdf_val;
			}
		}
		illum /= n_samples;
	}
	if (ray.depth < max_depth){
		illum += spec_reflect(ray, *bsdf, renderer, scene, sampler, pool);
		illum += spec_transmit(ray, *bsdf, renderer, scene, sampler, pool);
	}
	return illum;
}
*/

