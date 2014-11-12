#include <array>
#include "material/material.h"
#include "lights/light.h"
#include "lights/occlusion_tester.h"
#include "material/bsdf.h"
#include "renderer/renderer.h"
#include "integrator/bidir_path_integrator.h"

BidirPathIntegrator::BidirPathIntegrator(int min_depth, int max_depth) : min_depth(min_depth), max_depth(max_depth){}
Colorf BidirPathIntegrator::illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &r,
	DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const
{
	BSDF *cam_bsdf = nullptr, *light_bsdf = nullptr;
	Vector wo_cam, wo_light;
	Colorf cam_throughput = trace_camera(scene, renderer, r, dg, sampler, pool, cam_bsdf, wo_cam);
	Colorf light_illum = trace_light(scene, renderer, sampler, pool, light_bsdf, wo_light);
	if (cam_bsdf == nullptr){
		return light_illum;
	}
	if (light_bsdf == nullptr){
		return Colorf{0};
	}
	if (cam_throughput.is_black()){
		return Colorf{0};
	}
	if (light_illum.is_black()){
		return Colorf{0};
	}
	//TODO output color is way wrong, is this because we aren't permuting the paths and hooking every combination
	//up and sending light back along the camera subpath?
	OcclusionTester occlusion;
	occlusion.set_points(cam_bsdf->dg.point, light_bsdf->dg.point);
	if (!occlusion.occluded(scene)){
		return light_illum * cam_throughput;
	}
	return Colorf{0};
}
Colorf BidirPathIntegrator::trace_camera(const Scene &scene, const Renderer&, const RayDifferential &r,
	DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool, BSDF *&bsdf, Vector &w_o) const
{
	auto *bsdf_samples_u = pool.alloc_array<std::array<float, 2>>(max_depth + 1);
	auto *bsdf_samples_comp = pool.alloc_array<float>(max_depth + 1);
	sampler.get_samples(bsdf_samples_u, max_depth + 1);
	sampler.get_samples(bsdf_samples_comp, max_depth + 1);

	//Path throughput is the current product of bsdf values and geometry terms so far,
	//divided by their pdfs
	Colorf path_throughput{1};
	//Current ray to find next path point
	RayDifferential ray{r};
	//The current piece of geometry hit
	DifferentialGeometry dg_current = dg;
	for (int bounce = 0; ; ++bounce){
		if (!dg_current.node->get_material()){
			return path_throughput;
		}
		//Get the hit point information and determine our new path direction by sampling the BSDF
		dg_current.compute_differentials(ray);
		bsdf = dg_current.node->get_material()->get_bsdf(dg_current, pool);
		const Point &p = bsdf->dg.point;
		const Normal &n = bsdf->dg.normal;
		w_o = -ray.d;
		Vector w_i;
		float pdf_val = 0;
		BxDFTYPE sampled_type;
		Colorf f = bsdf->sample(w_o, w_i, bsdf_samples_u[bounce], bsdf_samples_comp[bounce],
			pdf_val, BxDFTYPE::ALL, &sampled_type);
		//If there's no further illumination or probability for this path, we can stop
		if (f.is_black() || pdf_val == 0){
			break;
		}
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
		if (!scene.get_root().intersect(ray, dg_current)){
			break;
		}
	}
	return path_throughput;
}
Colorf BidirPathIntegrator::trace_light(const Scene &scene, const Renderer&, Sampler &sampler,
	MemoryPool &pool, BSDF *&bsdf, Vector &w_o) const
{
	//We just need samples for picking the light and then sampling it to get an outgoing ray
	auto *l_samples_u = pool.alloc_array<std::array<float, 2>>(2);
	std::array<float, 2> l_samples_comp;
	auto *bsdf_samples_u = pool.alloc_array<std::array<float, 2>>(max_depth + 1);
	auto *bsdf_samples_comp = pool.alloc_array<float>(max_depth + 1);
	sampler.get_samples(l_samples_u, 2);
	sampler.get_samples(l_samples_comp.data(), l_samples_comp.size());
	sampler.get_samples(bsdf_samples_u, max_depth + 1);
	sampler.get_samples(bsdf_samples_comp, max_depth + 1);

	//Choose a light uniformly and sample a starting ray from it to begin the path
	int n_lights = scene.get_light_cache().size();
	if (n_lights == 0){
		return Colorf{0};
	}
	int light_num = static_cast<int>(l_samples_comp[0] * n_lights);
	light_num = std::min(light_num, n_lights - 1);
	//The unordered map isn't a random access container, so 'find' the light_num light
	auto lit = std::find_if(scene.get_light_cache().begin(), scene.get_light_cache().end(),
		[&light_num](const auto&){
			return light_num-- == 0;
		});

	Ray ray;
	Normal normal;
	float light_pdf_val = 0;
	//TODO: How would we be weighting the light path with along the BSDFs?
	Colorf illum = lit->second->sample(scene, LightSample{l_samples_u[0], l_samples_comp[1]}, l_samples_u[1],
		ray, normal, light_pdf_val);
	if (illum.is_black()){
		return illum;
	}
	Colorf path_throughput{1};

	//Find the first intersection and start tracing the light path
	DifferentialGeometry dg_current;
	if (!scene.get_root().intersect(ray, dg_current)){
		return illum / light_pdf_val;
	}
	bool specular_bounce = false;
	for (int bounce = 0; ; ++bounce){
		//Sample emissive objects on the first ray for directly visible ones or in the case of
		//specular bounces, as we don't compute them in estimate direct
		if (bounce == 0 || specular_bounce){
			const AreaLight *area_light = dg_current.node->get_area_light();
			if (area_light){
				illum += path_throughput * area_light->radiance(dg_current.point, dg_current.normal, -ray.d);
			}
		}
		if (!dg_current.node->get_material()){
			return illum * path_throughput / light_pdf_val;
		}
		//Get the hit point information and determine our new path direction by sampling the BSDF
		bsdf = dg_current.node->get_material()->get_bsdf(dg_current, pool);
		const Point &p = bsdf->dg.point;
		const Normal &n = bsdf->dg.normal;
		w_o = -ray.d;
		Vector w_i;
		float pdf_val = 0;
		BxDFTYPE sampled_type;
		Colorf f = bsdf->sample(w_o, w_i, bsdf_samples_u[bounce], bsdf_samples_comp[bounce],
			pdf_val, BxDFTYPE::ALL, &sampled_type);
		//If there's no further illumination or probability for this path, we can stop
		if (f.is_black() || pdf_val == 0){
			break;
		}
		specular_bounce = (sampled_type & BxDFTYPE::SPECULAR) != 0;
		path_throughput *= f * std::abs(w_i.dot(n)) / pdf_val;
		ray = Ray{p, w_i, ray, 0.001};

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
		if (!scene.get_root().intersect(ray, dg_current)){
			break;
		}
	}
	//TODO: Would this give the correct result? Or would we do a weighting
	//of the the light sample and path throughput samples?
	return illum * path_throughput / light_pdf_val;
}


