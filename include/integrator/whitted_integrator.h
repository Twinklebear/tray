#ifndef WHITTED_INTEGRATOR_H
#define WHITTED_INTEGRATOR_H

#include "surface_integrator.h"
#include "renderer/renderer.h"

/*
 * Surface integrator that uses Whitted recursive ray tracing algorithm
 * for computing illumination at a point on the surface
 */
class WhittedIntegrator : public SurfaceIntegrator {
	const int max_depth;

public:
	/*
	 * Create the Whitted integrator and set the max recursion depth for rays
	 */
	WhittedIntegrator(int max_depth);
	/*
	 * Compute the illumination at a point on a surface in the scene
	 */
	Colorf illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const override;
};

#endif

