#ifndef SURFACE_INTEGRATOR_H
#define SURFACE_INTEGRATOR_H

#include "scene.h"
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
		const RayDifferential &ray, const DifferentialGeometry &dg) const = 0;
};

#endif

