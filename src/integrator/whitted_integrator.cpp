#include "renderer/renderer.h"
#include "integrator/whitted_integrator.h"

WhittedIntegrator::WhittedIntegrator(int max_depth) : max_depth(max_depth){}
Colorf WhittedIntegrator::illumination(const Scene &scene, const Renderer &renderer,
	const RayDifferential &ray, const DifferentialGeometry &dg) const
{
	const Material *mat = dg.node->get_material();
	if (!mat){
		return Colorf{0.4};
	}
	Colorf illum;
	for (const auto &l : scene.get_light_cache()){
		if (l.second->type() == LIGHT::AMBIENT){
			illum += mat->shade(ray, dg, *l.second);
		}
		//Need the occlusion tester to clean this up some and proper light sampling code
		if (dg.normal.dot(-l.second->direction(dg.point)) > 0.f){
			DifferentialGeometry dummy;
			Ray r{dg.point, -l.second->direction(dg.point), 0.001};
			if (l.second->type() == LIGHT::DIRECT && !scene.get_root().intersect(r, dummy)){
				illum += mat->shade(ray, dg, *l.second);
			}
			else if (l.second->type() == LIGHT::POINT){
				r.max_t = 1;
				if (!scene.get_root().intersect(r, dummy)){
					illum += mat->shade(ray, dg, *l.second);
				}
			}
		}
	}
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
	return illum;
}

