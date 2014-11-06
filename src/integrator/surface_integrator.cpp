#include "integrator/surface_integrator.h"

Colorf SurfaceIntegrator::spec_reflect(const RayDifferential &ray, const BSDF &bsdf,
	const Renderer &renderer, const Scene &scene, Sampler &sampler)
{
	const Normal &n = bsdf.dg.normal;
	const Point &p = bsdf.dg.point;
	Vector wo = -ray.d;
	Vector wi;
	float pdf_val = 0;
	std::vector<std::array<float, 2>> u_sample(1);
	std::vector<float> c_sample(1);
	sampler.get_samples(u_sample);
	sampler.get_samples(c_sample);
	Colorf f = bsdf.sample(wo, wi, u_sample[0], c_sample[0], pdf_val,
		BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::SPECULAR));	
	//Compute the color reflected off the BSDF
	Colorf reflected{0};
	if (pdf_val > 0 && !f.luminance() == 0 && std::abs(wi.dot(n)) != 0){
		RayDifferential old_refl = ray.reflect(bsdf.dg);
		RayDifferential refl{p, wi, ray, 0.001};
		if (ray.has_differentials()){
			refl.rx = Ray{p + bsdf.dg.dp_dx, wi, ray, 0.001};
			refl.ry = Ray{p + bsdf.dg.dp_dy, wi, ray, 0.001};
			//We compute dn_dx and dn_dy as described in PBR since we're following their differential
			//geometry method, the rest of the computation is directly from Igehy's paper
			auto dn_dx = Vector{bsdf.dg.dn_du * bsdf.dg.du_dx + bsdf.dg.dn_dv * bsdf.dg.dv_dx};
			auto dn_dy = Vector{bsdf.dg.dn_du * bsdf.dg.du_dy + bsdf.dg.dn_dv * bsdf.dg.dv_dy};
			auto dd_dx = -ray.rx.d - wo;
			auto dd_dy = -ray.ry.d - wo;
			float ddn_dx = dd_dx.dot(n) + wo.dot(dn_dx);
			float ddn_dy = dd_dy.dot(n) + wo.dot(dn_dy);
			refl.rx.d = wi - dd_dx + 2 * Vector{wo.dot(n) * dn_dx + Vector{ddn_dx * n}};
			refl.ry.d = wi - dd_dy + 2 * Vector{wo.dot(n) * dn_dy + Vector{ddn_dy * n}};
		}
		Colorf li = renderer.illumination(refl, scene, sampler);
		reflected = f * li * std::abs(wi.dot(n)) / pdf_val;
		Vector dir = refl.d.normalized();
		reflected = Colorf{dir.x, dir.y, dir.z};
		reflected = (reflected + Colorf{1}) / 2;
	}
	return reflected;
}
Colorf SurfaceIntegrator::spec_transmit(const RayDifferential &ray, const BSDF &bsdf,
	const Renderer &renderer, const Scene &scene, Sampler &sampler)
{
	/*
	Vector wo = -ray.d;
	Vector wi;
	float pdf_val = 0;
	std::vector<std::array<float, 2>> u_sample(1);
	std::vector<float> c_sample(1);
	sampler.get_samples(u_sample);
	sampler.get_samples(c_sample);
	Colorf f = bsdf.sample(wo, wi, u_sample[0], c_sample[0], pdf_val,
		BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::TRANSMISSION));	
	//Compute the color transmitted through the BSDF
	Colorf transmitted{0};
	if (pdf_val > 0 && !f.luminance() == 0 && std::abs(wi.dot(dg.normal)) != 0){
		RayDifferential refl_ray = ray.refract(dg, bsdf.eta);
		Colorf li = renderer.illumination(refl_ray, scene, sampler);
		transmitted = f * li * std::abs(wi.dot(dg.normal)) / pdf_val;
	}
	return transmitted;
	*/
	return Colorf{0};
}

