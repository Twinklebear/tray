#ifndef PATH_INTEGRATOR_H
#define PATH_INTEGRATOR_H

#include "surface_integrator.h"
#include "renderer/renderer.h"

/*
 * Computes the illumination at a point using path tracing
 */
class PathIntegrator : public SurfaceIntegrator {
	const int min_depth, max_depth;

public:
	/*
	 * Create the Path tracing integrator and set the min and max depth for paths
	 * after a path reaches min depth we'll randomly consider terminating it,
	 * if a path reaches max_depth it's terminated
	 */
	PathIntegrator(int min_depth, int max_depth);
	/*
	 * Compute the illumination at a point on a surface in the scene
	 */
	Colorf illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const override;
};

#endif

