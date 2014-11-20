#include <cassert>
#include <algorithm>
#include <cmath>
#include "scene.h"
#include "monte_carlo/util.h"
#include "monte_carlo/distribution1d.h"
#include "integrator/surface_integrator.h"

void SurfaceIntegrator::preprocess(const Scene&){}
Colorf SurfaceIntegrator::spec_reflect(const RayDifferential &ray, const BSDF &bsdf, const Renderer &renderer,
	const Scene &scene, Sampler &sampler, MemoryPool &pool)
{
	const Normal &n = bsdf.dg.normal;
	const Point &p = bsdf.dg.point;
	Vector w_o = -ray.d;
	Vector w_i;
	float pdf_val = 0;
	int n_samples = 1;
	std::array<float, 2> u_sample;
	float c_sample = 0;
	sampler.get_samples(&u_sample, n_samples);
	sampler.get_samples(&c_sample, n_samples);
	//Compute the color reflected off the BSDF
	Colorf reflected{0};
	Colorf f = bsdf.sample(w_o, w_i, u_sample, c_sample, pdf_val,
		BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::SPECULAR));
	if (pdf_val > 0 && !f.is_black() && std::abs(w_i.dot(n)) != 0){
		RayDifferential refl{p, w_i, ray, 0.001};
		if (ray.has_differentials()){
			refl.rx = Ray{p + bsdf.dg.dp_dx, w_i, ray, 0.001};
			refl.ry = Ray{p + bsdf.dg.dp_dy, w_i, ray, 0.001};
			//We compute dn_dx and dn_dy as described in PBR since we're following their differential
			//geometry method, the rest of the computation is directly from Igehy's paper
			auto dn_dx = Vector{bsdf.dg.dn_du * bsdf.dg.du_dx + bsdf.dg.dn_dv * bsdf.dg.dv_dx};
			auto dn_dy = Vector{bsdf.dg.dn_du * bsdf.dg.du_dy + bsdf.dg.dn_dv * bsdf.dg.dv_dy};
			auto dd_dx = -ray.rx.d - w_o;
			auto dd_dy = -ray.ry.d - w_o;
			float ddn_dx = dd_dx.dot(n) + w_o.dot(dn_dx);
			float ddn_dy = dd_dy.dot(n) + w_o.dot(dn_dy);
			refl.rx.d = w_i - dd_dx + 2 * Vector{w_o.dot(n) * dn_dx + Vector{ddn_dx * n}};
			refl.ry.d = w_i - dd_dy + 2 * Vector{w_o.dot(n) * dn_dy + Vector{ddn_dy * n}};
		}
		Colorf li = renderer.illumination(refl, scene, sampler, pool);
		reflected = f * li * std::abs(w_i.dot(n)) / pdf_val;
	}
	return reflected / n_samples;
}
Colorf SurfaceIntegrator::spec_transmit(const RayDifferential &ray, const BSDF &bsdf, const Renderer &renderer,
	const Scene &scene, Sampler &sampler, MemoryPool &pool)
{
	const Normal &n = bsdf.dg.normal;
	const Point &p = bsdf.dg.point;
	Vector w_o = -ray.d;
	Vector w_i;
	float pdf_val = 0;
	std::array<float, 2> u_sample;
	float c_sample;
	sampler.get_samples(&u_sample, 1);
	sampler.get_samples(&c_sample, 1);
	Colorf f = bsdf.sample(w_o, w_i, u_sample, c_sample, pdf_val,
		BxDFTYPE(BxDFTYPE::TRANSMISSION | BxDFTYPE::SPECULAR));
	//Compute the color transmitted through the BSDF
	Colorf transmitted{0};
	if (pdf_val > 0 && f.is_black() && std::abs(w_i.dot(n)) != 0){
		RayDifferential refr_ray{p, w_i, ray, 0.001};
		if (ray.has_differentials()){
			refr_ray.rx = Ray{p + bsdf.dg.dp_dx, w_i, ray, 0.001};
			refr_ray.ry = Ray{p + bsdf.dg.dp_dy, w_i, ray, 0.001};

			float eta = w_o.dot(n) < 0 ? 1 / bsdf.eta : bsdf.eta;
			//We compute dn_dx and dn_dy as described in PBR since we're following their differential
			//geometry method, the rest of the computation is directly from Igehy's paper
			auto dn_dx = Vector{bsdf.dg.dn_du * bsdf.dg.du_dx + bsdf.dg.dn_dv * bsdf.dg.dv_dx};
			auto dn_dy = Vector{bsdf.dg.dn_du * bsdf.dg.du_dy + bsdf.dg.dn_dv * bsdf.dg.dv_dy};
			auto dd_dx = -ray.rx.d - w_o;
			auto dd_dy = -ray.ry.d - w_o;
			float ddn_dx = dd_dx.dot(n) + w_o.dot(dn_dx);
			float ddn_dy = dd_dy.dot(n) + w_o.dot(dn_dy);

			float mu = eta * ray.d.dot(n) - w_i.dot(n);
			float dmu_dx = (eta - eta * eta * ray.d.dot(n) / w_i.dot(n)) * ddn_dx;
			float dmu_dy = (eta - eta * eta * ray.d.dot(n) / w_i.dot(n)) * ddn_dy;
			refr_ray.rx.d = w_i + eta * dd_dx - Vector{mu * dn_dx + Vector{dmu_dx * n}};
			refr_ray.ry.d = w_i + eta * dd_dy - Vector{mu * dn_dy + Vector{dmu_dy * n}};
		}
		Colorf li = renderer.illumination(refr_ray, scene, sampler, pool);
		transmitted = f * li * std::abs(w_i.dot(n)) / pdf_val;
	}
	return transmitted;
}
Colorf SurfaceIntegrator::uniform_sample_one_light(const Scene &scene, const Renderer &renderer, const Point &p,
	const Normal &n, const Vector &w_o, const BSDF &bsdf, const LightSample &l_sample, const BSDFSample &bsdf_sample)
{
	int n_lights = scene.get_light_cache().size();
	if (n_lights == 0){
		return Colorf{0};
	}
	int light_num = static_cast<int>(l_sample.light * n_lights);
	light_num = std::min(light_num, n_lights - 1);
	//The unordered map isn't a random access container, so 'find' the light_num light
	auto lit = std::find_if(scene.get_light_cache().begin(), scene.get_light_cache().end(),
		[&light_num](const auto&){
			return light_num-- == 0;
		});
	assert(lit != scene.get_light_cache().end());
	return n_lights * estimate_direct(scene, renderer, p, n, w_o, bsdf, *lit->second, l_sample,
		bsdf_sample, BxDFTYPE(BxDFTYPE::ALL & ~BxDFTYPE::SPECULAR));
}
Colorf SurfaceIntegrator::estimate_direct(const Scene &scene, const Renderer &, const Point &p,
	const Normal &n, const Vector &w_o, const BSDF &bsdf, const Light &light, const LightSample &l_sample,
	const BSDFSample &bsdf_sample, BxDFTYPE flags)
{
	//We sample both the light source and the BSDF and weight them accordingly to sample both well
	Colorf direct_light;
	Vector w_i;
	float pdf_light = 0, pdf_bsdf = 0;
	OcclusionTester occlusion;
	//Sample the light
	Colorf li = light.sample(p, l_sample, w_i, pdf_light, occlusion);
	if (pdf_light > 0 && !li.is_black()){
		Colorf f = bsdf(w_o, w_i, flags);
		if (!f.is_black() && !occlusion.occluded(scene)){
			//If we have a delta distribution in the light we don't do MIS as it'd be incorrect
			//Otherwise we do MIS using the power heuristic
			if (light.delta_light()){
				direct_light += f * li * std::abs(w_i.dot(n)) / pdf_light;
			}
			else {
				pdf_bsdf = bsdf.pdf(w_o, w_i, flags);
				float w = power_heuristic(1, pdf_light, 1, pdf_bsdf);
				direct_light += f * li * std::abs(w_i.dot(n)) * w / pdf_light;
			}
		}
	}
	//Sample the BSDF in the same manner as the light
	if (!light.delta_light()){
		BxDFTYPE sampled_bxdf;
		Colorf f = bsdf.sample(w_o, w_i, bsdf_sample.u, bsdf_sample.comp, pdf_bsdf, flags, &sampled_bxdf);
		if (pdf_bsdf > 0 && !f.is_black()){
			//Handle delta distributions in the BSDF the same way we did for the light
			float weight = 1;
			if (!(sampled_bxdf & BxDFTYPE::SPECULAR)){
				pdf_light = light.pdf(p, w_i);
				if (pdf_light == 0){
					return direct_light;
				}
				weight = power_heuristic(1, pdf_bsdf, 1, pdf_light);
			}
			//Find out if the ray along w_i actually hits the light source
			DifferentialGeometry dg;
			Colorf li;
			RayDifferential ray{p, w_i, 0.001};
			if (scene.get_root().intersect(ray, dg)){
				if (dg.node->get_area_light() == &light){
					li = dg.node->get_area_light()->radiance(dg.point, dg.normal, -w_i);
				}
			}
			if (!li.is_black()){
				direct_light += f * li * std::abs(w_i.dot(n)) * weight / pdf_bsdf;
			}
		}
	}
	return direct_light;
}
Distribution1D SurfaceIntegrator::light_sampling_cdf(const Scene &scene){
	std::vector<float> light_power(scene.get_light_cache().size());
	std::transform(scene.get_light_cache().begin(), scene.get_light_cache().end(), light_power.begin(),
		[&](const auto &l){
			return l.second->power(scene).luminance();
		});
	return Distribution1D{light_power};
}

