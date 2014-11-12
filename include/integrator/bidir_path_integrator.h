#ifndef BIDIR_PATH_INTEGRATOR_H
#define BIDIR_PATH_INTEGRATOR_H

#include "surface_integrator.h"
#include "renderer/renderer.h"

/*
 * Surface integrator that uses bidirectional path tracing for computing illumination
 * at a a point on the surface
 */
class BidirPathIntegrator : public SurfaceIntegrator {
	const int min_depth, max_depth;

public:
	/*
	 * Create the path tracing integrator and set the min and max depth for paths
	 * rays are randomly stopped by Russian roulette after reaching min_depth and are stopped
	 * at max_depth
	 */
	BidirPathIntegrator(int min_depth, int max_depth);
	/*
	 * Compute the illumination at a point on a surface in the scene
	 */
	Colorf illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const override;

private:
	/*
	 * Trace a camera path starting from the first hit at dg returning the throughput of the
	 * path and the BSDF of the last object hit and the direction we hit it from so that we
	 * can connect the path to a light path
	 */
	Colorf trace_camera(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool, BSDF *&bsdf, Vector &w_o) const;
	/*
	 * Trace a light path returning the illumination along the path and the BSDF of
	 * of the last object hit and the direction we hit it from so that we
	 */
	Colorf trace_light(const Scene &scene, const Renderer &renderer, Sampler &sampler,
		MemoryPool &pool, BSDF *&bsdf, Vector &w_o) const;
};

#endif

