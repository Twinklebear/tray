#include "integrator/surface_integrator.h"

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
			BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::SPECULAR | BxDFTYPE::GLOSSY));	
	if (pdf_val > 0 && !f.luminance() == 0 && std::abs(w_i.dot(n)) != 0){
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
		reflected += f * li * std::abs(w_i.dot(n)) / pdf_val;
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
	if (pdf_val > 0 && !f.luminance() == 0 && std::abs(w_i.dot(n)) != 0){
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

