#ifndef EMISSION_INTEGRATOR_H
#define EMISSION_INTEGRATOR_H

#include "volume_integrator.h"

/*
 * The EmissionIntegrator is a volume integrator that only
 * accounts for absoprtion and emission effects of the volumes
 * being traversed
 * See: PBR
 */
class EmissionIntegrator : public VolumeIntegrator {
	//Distance between samples to take of the volume when integrating
	float step_size;

public:
	/*
	 * Construct the emission integrator specifying the distance between
	 * samples when integrating over the volumes in the scene
	 */
	EmissionIntegrator(float step_size);
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

