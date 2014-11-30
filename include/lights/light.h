#ifndef LIGHT_H
#define LIGHT_H

#include <array>
#include "cache.h"
#include "linalg/transform.h"
#include "film/color.h"
#include "occlusion_tester.h"

class Scene;

/*
 * Struct to easily pass light samples for choosing a light and then sampling the light
 */
struct LightSample {
	std::array<float, 2> u;
	float light;
};

/*
 * Base class for lights, stores the transformation from world space to light space
 * and the number of samples that should be taken when sampling this light
 */
class Light {
protected:
	//Transforms from light to world space and world to light space
	const Transform to_world, to_light;

public:
	//Specify number of samples to take of the light when uniformly sampling all lights
	const int n_samples;

	/*
	 * Create the light with some transformation to world space
	 * and desired number of samples to be taken
	 */
	Light(const Transform &to_world, int n_samples = 1);
	/*
	 * Sample the illumination from the light arriving at the point
	 * returns the color along with the incident light direction for the point,
	 * the PDF of the point that was sampled and fills out the occlusion tester
	 * for shadow testing if the color returned wasn't black
	 */
	virtual Colorf sample(const Point &p, const LightSample &lsample,
		Vector &w_i, float &pdf_val, OcclusionTester &occlusion) const = 0;
	/*
	 * Sample a light ray from the distribution of rays leaving the light
	 * returning the ray, normal on the light surface where the ray was emitted
	 * and pdf
	 */
	virtual Colorf sample(const Scene &scene, const LightSample &lsample,
		const std::array<float, 2> &b, Ray &ray, Normal &normal, float &pdf_val) const = 0;
	/*
	 * Compute the total power emitted by the light in the scene
	 */
	virtual Colorf power(const Scene &scene) const = 0;
	/*
	 * Check if this light is a delta light, eg. a point or directional light
	 */
	virtual bool delta_light() const = 0;
	/*
	 * Compute the PDF for sampling the point with the incident direction wi
	 */
	virtual float pdf(const Point &p, const Vector &w_i) const = 0;
};

typedef Cache<Light> LightCache;

#endif

