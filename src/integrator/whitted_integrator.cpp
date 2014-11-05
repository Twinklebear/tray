#include "material/pbr_material.h"
#include "lights/pbr_light.h"
#include "lights/occlusion_tester.h"
#include "material/bsdf.h"
#include "renderer/renderer.h"
#include "integrator/whitted_integrator.h"

WhittedIntegrator::WhittedIntegrator(int max_depth) : max_depth(max_depth){}
Colorf WhittedIntegrator::illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
	const DifferentialGeometry &dg, Sampler &sampler) const
{
	const PBRMaterial *mat = dg.node->get_material();
	if (!mat){
		return Colorf{0.4};
	}
	BSDF bsdf = mat->get_bsdf(dg);

	Colorf illum;
	Vector wo = -ray.d;
	//Compute the incident light from all lights in the scene
	for (const auto &l : scene.get_light_cache()){
		Vector wi;
		float pdf_val = 0;
		OcclusionTester occlusion;
		Colorf li = l.second->sample(bsdf.dg.point, {sampler.random_float(), sampler.random_float()},
			wi, pdf_val, occlusion);
		//If there's no light or no probability for this sample there's no illumination
		if (li.luminance() == 0 || pdf_val == 0){
			continue;
		}
		Colorf c = bsdf(wo, wi);
		if (c.luminance() != 0 && !occlusion.occluded(scene)){
			illum += c * li * std::abs(wi.dot(bsdf.dg.normal)) / pdf_val;
		}
	}
	/*
	if (ray.depth < max_depth){
		//TODO: This should be cleaned up similar to what PBR does with specular reflect & transmit
		//Track reflection contribution from Fresnel term to be incorporated
		//into reflection calculation
		Colorf fresnel_refl;
		if (mat->is_transparent()){
			float eta = 0;
			Vector n;
			//Compute proper refractive index ratio and set normal to be on same side
			//as indicident ray for refraction computation when entering/exiting material
			if (dg.hit_side == HITSIDE::FRONT){
				eta = 1.f / mat->refractive_idx();
				n = Vector{dg.normal.normalized()};
			}
			else {
				eta = mat->refractive_idx();
				n = -Vector{dg.normal.normalized()};
			}
			//Compute Schlick's approximation to find amount reflected and transmitted at the surface
			//Note that we use -ray.d here since V should be from point -> camera and we use
			//refl_dir as the "light" direction since that's the light reflection we're interested in
			Vector refl_dir = ray.d - 2 * n.dot(ray.d) * n;
			Vector h = (refl_dir - ray.d).normalized();
			float r = std::pow((mat->refractive_idx() - 1) / (mat->refractive_idx() + 1), 2.f);
			r = r + (1 - r) * std::pow(1 - h.dot(-ray.d), 5);

			//Compute the contribution from light refracting through the object and check for total
			//internal reflection
			float c = -n.dot(ray.d);
			float root = 1 - eta * eta * (1 - c * c);
			if (root > 0){
				RayDifferential refr = ray.refract(dg, n, eta);
				//Account for absorption by the object if the refraction ray we're casting is entering it
				Colorf refr_col = renderer.illumination(refr, scene) * mat->refractive(dg) * (1 - r);
				if (dg.hit_side == HITSIDE::FRONT){
					Colorf absorbed = mat->absorbed(dg);
					illum += refr_col * Colorf{std::exp(-refr.max_t * absorbed.r),
						std::exp(-refr.max_t * absorbed.g), std::exp(-refr.max_t * absorbed.b)};
				}
				else {
					illum += refr_col;
				}
			}
			//In the case of total internal reflection all the contribution is from the reflected term
			else {
				r = 1;
			}
			//Add Fresnel reflection contribution to be used when computing reflection
			fresnel_refl = mat->refractive(dg) * r;
		}
		if (mat->is_reflective() || fresnel_refl != Colorf{0, 0, 0}){
			Colorf refl_col = mat->reflective(dg) + fresnel_refl;
			//Reflect and cast ray
			RayDifferential refl = ray.reflect(dg);
			illum += renderer.illumination(refl, scene) * refl_col;
		}
	}
	*/
	return illum;
}

