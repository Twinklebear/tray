#include "integrator/surface_integrator.h"

Colorf SurfaceIntegrator::spec_reflect(const RayDifferential &ray, const DifferentialGeometry &dg,
	const BSDF &bsdf, const Renderer &renderer, const Scene &scene, Sampler &sampler)
{
	return Colorf{0};
}

