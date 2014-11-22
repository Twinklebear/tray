#ifndef SURFACE_INTEGRATOR_H
#define SURFACE_INTEGRATOR_H

#include "samplers/sampler.h"
#include "renderer/renderer.h"
#include "linalg/ray.h"
#include "geometry/differential_geometry.h"
#include "memory_pool.h"
#include "film/color.h"
#include "material/bsdf.h"
#include "lights/light.h"
#include "monte_carlo/distribution1d.h"

class Scene;

/*
 * Compute the illumnation at a point on a surface in the scene
 */
class SurfaceIntegrator {
public:
	/*
	 * Pre-process the scene peforming any pre-process setup steps needed by the surface
	 * integrator. Eg. build the photon maps for photon mapping. The default implementation
	 * does nothing
	 */
	virtual void preprocess(const Scene &scene);
	/*
	 * Compute the illumination at a point on the surface in the scene
	 */
	virtual Colorf illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const = 0;
	/*
	 * Utility function to compute the specularly reflected light off of
	 * some geometry we hit
	 */
	static Colorf spec_reflect(const RayDifferential &ray, const BSDF &bsdf, const Renderer &renderer,
		const Scene &scene, Sampler &sampler, MemoryPool &pool);
	/*
	 * Utility function to compute the specularaly transmitted light coming
	 * through some geometry we hit
	 */
	static Colorf spec_transmit(const RayDifferential &ray, const BSDF &bsdf, const Renderer &renderer,
		const Scene &scene, Sampler &sampler, MemoryPool &pool);
	/*
	 * Utility function to uniformly sample all the lights in the scene by estimating their direct
	 * illumination at the bsdf
	 */
	static Colorf uniform_sample_all_lights(const Scene &scene, const Renderer &renderer, const Point &p,
		const Normal &n, const Vector &w_o, const BSDF &bsdf, Sampler &sampler, MemoryPool &pool);
	/*
	 * Utility function to uniformly samply one light in the scene illuminating the BSDF
	 */
	static Colorf uniform_sample_one_light(const Scene &scene, const Renderer &renderer, const Point &p,
		const Normal &n, const Vector &w_o, const BSDF &bsdf, const LightSample &l_sample,
		const BSDFSample &bsdf_sample);
	/*
	 * Estimate the direct light contribution from the light passed to the BSDF
	 */
	static Colorf estimate_direct(const Scene &scene, const Renderer &renderer, const Point &p,
		const Normal &n, const Vector &w_o, const BSDF &bsdf, const Light &light, const LightSample &l_sample,
		const BSDFSample &bsdf_sample, BxDFTYPE flags);
	/*
	 * Compute and return the light sampling CDF distribution
	 */
	static Distribution1D light_sampling_cdf(const Scene &scene);
};

#endif

