#ifndef SURFACE_INTEGRATOR_H
#define SURFACE_INTEGRATOR_H

#include "scene.h"
#include "samplers/sampler.h"
#include "renderer/renderer.h"
#include "linalg/ray.h"
#include "geometry/differential_geometry.h"
#include "film/color.h"

/*
 * Compute the illumnation at a point on a surface in the scene
 */
class SurfaceIntegrator {
public:
	/*
	 * Compute the illumination at a point on the surface in the scene
	 */
	virtual Colorf illumination(const Scene &scene, const Renderer &renderer,
		const RayDifferential &ray, const DifferentialGeometry &dg, Sampler &sampler) const = 0;
	/*
	 * Utility function to compute the specularly reflected light off of
	 * some geometry we hit
	 */
	static Colorf spec_reflect(const RayDifferential &ray, const DifferentialGeometry &dg,
		const BSDF &bsdf, const Renderer &renderer, const Scene &scene, Sampler &sampler);
};

#endif

