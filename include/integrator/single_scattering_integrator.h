#ifndef SINGLE_SCATTERING_INTEGRATOR_H
#define SINGLE_SCATTERING_INTEGRATOR_H

#include "volume_integrator.h"

/*
 * The SingleScatteringIntegrator is a volume integrator that
 * accounts for absoprtion and emission effects of the volumes
 * being traversed along with in scattering effects
 * See: PBR
 */
class SingleScatteringIntegrator : public VolumeIntegrator {
	//Distance between samples to take of the volume when integrating
	float step_size;

public:
	/*
	 * Construct the single scattering integrator specifying the distance between
	 * samples when integrating over the volumes in the scene
	 */
	SingleScatteringIntegrator(float step_size);
	/*
	 * Compute the radiance arriving at the point p along the direction w
	 * due to participating media in the scene. Also returns the transmittance
	 * along the ray through transmit
	 */
	virtual Colorf radiance(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		Sampler &sampler, MemoryPool &pool, Colorf &transmit) const override;
	/*
	 * Compute the beam transmittance for line segment along the ray from min_t to max_t
	 */
	virtual Colorf transmittance(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		Sampler &sampler, MemoryPool &pool) const override;
};

#endif


