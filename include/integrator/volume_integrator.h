#ifndef VOLUME_INTEGRATOR_H
#define VOLUME_INTEGRATOR_H

#include "samplers/sampler.h"
#include "renderer/renderer.h"
#include "linalg/ray.h"
#include "memory_pool.h"
#include "film/color.h"

class Scene;

/*
 * Interface for volume integrators to implement
 */
class VolumeIntegrator {
public:
	/*
	 * Compute the radiance arriving at the point p along the direction w
	 * due to participating media in the scene. Also returns the transmittance
	 * along the ray through transmit
	 */
	virtual Colorf radiance(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		Sampler &sampler, MemoryPool &pool, Colorf &transmit) const = 0;
	/*
	 * Compute the beam transmittance for line segment along the ray from min_t to max_t
	 */
	virtual Colorf transmittance(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		Sampler &sampler, MemoryPool &pool) const = 0;
};

#endif

