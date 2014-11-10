#include <array>
#include "material/material.h"
#include "lights/light.h"
#include "lights/occlusion_tester.h"
#include "material/bsdf.h"
#include "renderer/renderer.h"
#include "integrator/whitted_integrator.h"

WhittedIntegrator::WhittedIntegrator(int max_depth) : max_depth(max_depth){}
Colorf WhittedIntegrator::illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
	DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const
{
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

	std::array<float, 2> l_samples;
	//Compute the incident light from all lights in the scene
	for (const auto &l : scene.get_light_cache()){
		sampler.get_samples(&l_samples, 1);
		Vector w_i;
		float pdf_val = 0;
		OcclusionTester occlusion;
		Colorf li = l.second->sample(bsdf->dg.point, l_samples, w_i, pdf_val, occlusion);
		//If there's no light or no probability for this sample there's no illumination
		if (li.luminance() == 0 || pdf_val == 0){
			continue;
		}
		Colorf c = (*bsdf)(w_o, w_i);
		if (!c.is_black() && !occlusion.occluded(scene)){
			illum += c * li * std::abs(w_i.dot(bsdf->dg.normal)) / pdf_val;
		}
	}
	if (ray.depth < max_depth){
		illum += spec_reflect(ray, *bsdf, renderer, scene, sampler, pool);
		illum += spec_transmit(ray, *bsdf, renderer, scene, sampler, pool);
	}
	return illum;
}

