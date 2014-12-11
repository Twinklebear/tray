#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "samplers/sampler.h"
#include "linalg/ray.h"
#include "memory_pool.h"

class SurfaceIntegrator;
class VolumeIntegrator;
class Scene;

/*
 * Interface for renderers, given a ray to trace and a scene
 * to trace it in returns the illumination along the ray
 */
class Renderer {
	std::unique_ptr<SurfaceIntegrator> surface_integrator;
	std::unique_ptr<VolumeIntegrator> volume_integrator;

public:
	Renderer(std::unique_ptr<SurfaceIntegrator> surface_integrator, std::unique_ptr<VolumeIntegrator> volume_integrator);
	/*
	 * Have the renderer and its integrators perform any needed pre-processing of the scene
	 */
	void preprocess(const Scene &scene);
	/*
	 * Compute the incident radiance along the ray in the scene
	 * The default implementation simply calls the surface integrator on
	 * the hit geometry to compute the illumination
	 */
	virtual Colorf illumination(RayDifferential &ray, const Scene &scene, Sampler &sampler, MemoryPool &pool) const;
	/*
	 * Compute the beam transmittance for line segment along the ray from min_t to max_t using the
	 * volume integrator, if any. If no volume integrator is being used, simply returns 1 (eg. air)
	 */
	virtual Colorf transmittance(const Scene &scene, const RayDifferential &ray, Sampler &sampler, MemoryPool &pool) const;

};

#endif

