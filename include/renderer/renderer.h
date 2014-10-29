#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "linalg/ray.h"
#include "scene.h"

class SurfaceIntegrator;

/*
 * Interface for renderers, given a ray to trace and a scene
 * to trace it in returns the illumination along the ray
 */
class Renderer {
	std::unique_ptr<SurfaceIntegrator> surface_integrator;

public:
	Renderer(std::unique_ptr<SurfaceIntegrator> surface_integrator);
	/*
	 * Compute the incident radiance along the ray in the scene
	 * The default implementation simply calls the surface integrator on
	 * the hit geometry to compute the illumination
	 */
	virtual Colorf illumination(RayDifferential &ray, const Scene &scene) const;
};

#endif

