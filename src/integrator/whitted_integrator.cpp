#include <vector>
#include <array>
#include "material/pbr_material.h"
#include "lights/pbr_light.h"
#include "lights/occlusion_tester.h"
#include "material/bsdf.h"
#include "renderer/renderer.h"
#include "integrator/whitted_integrator.h"

WhittedIntegrator::WhittedIntegrator(int max_depth) : max_depth(max_depth){}
Colorf WhittedIntegrator::illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
	const DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const
{
	const PBRMaterial *mat = dg.node->get_material();
	if (!mat){
		return Colorf{0.4};
	}
	BSDF *bsdf = mat->get_bsdf(dg, pool);

	Colorf illum;
	Vector wo = -ray.d;
	std::vector<std::array<float, 2>> light_sample(1);
	//Compute the incident light from all lights in the scene
	for (const auto &l : scene.get_light_cache()){
		sampler.get_samples(light_sample);
		Vector wi;
		float pdf_val = 0;
		OcclusionTester occlusion;
		Colorf li = l.second->sample(bsdf->dg.point, light_sample[0], wi, pdf_val, occlusion);
		//If there's no light or no probability for this sample there's no illumination
		if (li.luminance() == 0 || pdf_val == 0){
			continue;
		}
		Colorf c = (*bsdf)(wo, wi);
		if (c.luminance() != 0 && !occlusion.occluded(scene)){
			illum += c * li * std::abs(wi.dot(bsdf->dg.normal)) / pdf_val;
		}
	}
	if (ray.depth < max_depth){
		illum += spec_reflect(ray, *bsdf, renderer, scene, sampler, pool);
		illum += spec_transmit(ray, *bsdf, renderer, scene, sampler, pool);
	}
	return illum;
}

