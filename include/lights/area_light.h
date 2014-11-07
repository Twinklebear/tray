#ifndef AREA_LIGHT_H
#define AREA_LIGHT_H

#include <memory>
#include "geometry/sphere.h"
#include "pbr_light.h"

/*
 * Diffuse area light that can be attached to some arbitrary geometry
 * to create an emissive version of the object
 * TODO: Currently only spheres are supported
 */
class AreaLight : public PBRLight {
	Colorf emit;
	std::unique_ptr<Sphere> geometry;
	float surface_area;

public:
	/*
	 * Construct the area light with some transformation to be applied to
	 * the sphere it's emitting from
	 */
	AreaLight(const Transform &to_world, const Colorf &emit, int n_samples, float radius);
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
	 */
	float pdf(const Point &p, const Vector &wi) const override;
	/*
	 * Compute the light's emitted radiance in the outgoing direction, w
	 * from the point on the surface with normal n
	 */
	Colorf radiance(const Point &p, const Normal &n, const Vector &w) const;
};

#endif

