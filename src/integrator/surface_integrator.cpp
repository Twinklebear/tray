#include "integrator/surface_integrator.h"

Colorf SurfaceIntegrator::spec_reflect(const RayDifferential &ray, const DifferentialGeometry &dg,
	const BSDF &bsdf, const Renderer &renderer, const Scene &scene, std::minstd_rand &rng)
{
	Vector wo = -ray.d;
	Vector wi;
	float pdf_val = 0;
	return Colorf{0};
}

