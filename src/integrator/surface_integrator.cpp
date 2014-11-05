#include "integrator/surface_integrator.h"

Colorf SurfaceIntegrator::spec_reflect(const RayDifferential &ray, const DifferentialGeometry &dg,
	const BSDF &bsdf, const Renderer &renderer, const Scene &scene, Sampler &sampler)
{
	Vector wo = ray.d;
	Vector wi;
	float pdf_val = 0;
	std::vector<std::array<float, 2>> u_sample(1);
	std::vector<float> c_sample(1);
	sampler.get_samples(u_sample);
	sampler.get_samples(c_sample);
	Colorf f = bsdf.sample(wo, wi, u_sample[0], c_sample[0], pdf_val, BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::SPECULAR));	
	//Compute the color reflected off the BSDF
	Colorf reflected{0};
	if (pdf_val > 0 && !f.luminance() == 0 && std::abs(wi.dot(dg.normal)) != 0){
		RayDifferential refl_ray = ray.reflect(dg);
		Colorf li = renderer.illumination(refl_ray, scene, sampler);
		reflected = f * li * std::abs(wi.dot(dg.normal)) / pdf_val;
	}
	return reflected;
}

