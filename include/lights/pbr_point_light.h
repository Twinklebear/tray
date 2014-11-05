#ifndef PBR_POINT_LIGHT_H
#define PBR_POINT_LIGHT_H

#include "film/color.h"
#include "pbr_light.h"

/*
 * Describes a simple point light
 */
class PBRPointLight : public PBRLight {
	const Point position;
	const Colorf intensity;

public:
	/*
	 * Create a point light with some transformation to world space
	 * and the desired intensity
	 */
	PBRPointLight(const Transform &to_world, const Colorf &intensity);
	/*
	 * Sample the illumination from the light arriving at the point
	 * returns the color along with the incident light direction for the point,
	 * the PDF of the point that was sampled and fills out the occlusion tester
	 * for shadow testing if the color returned wasn't black
	 */
	Colorf sample(const Point &p, const std::array<float, 2> &lsample,
		Vector &wi, float &pdf_val, OcclusionTester &occlusion) const override;
	/*
	 * Compute the total power emitted by the light in the scene
	 */
	Colorf power(const Scene &scene) const override;
	/*
	 * Check if this light is a delta light, eg. a point or directional light
	 */
	bool delta_light() const override;
	/*
	 * Compute the PDF for sampling the point with the incident direction wi
	 * since there's no chance of sampling the delta distribution randomly this
	 * just returns 0
	 */
	float pdf(const Point &p, const Vector &wi) const override;
};

#endif

