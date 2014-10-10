#ifndef SPHERICAL_MAPPING_H
#define SPHERICAL_MAPPING_H

#include "linalg/point.h"
#include "linalg/transform.h"
#include "texture_mapping.h"

/*
 * Mapping that projects the hit point to a point on a sphere around the object
 * and uses the sphere parameterization as the texture coordinates
 */
class SphericalMapping : public TextureMapping {
	Transform transform;

public:
	SphericalMapping(const Transform &transform);
	/*
	 * Compute the texture sample position and derivatives for the
	 * differential geometry being rendered
	 */
	TextureSample map(const DifferentialGeometry &dg) const override;

private:
	/*
	 * Project a point onto the sphere placed around the object and return the s,t
	 * coordinates of the sphere at that point
	 */
	void sphere_project(const Point &p, float &s, float &t) const;
};

#endif

