#ifndef CONE_H
#define CONE_H

#include "geometry.h"

/*
 * A cone with some radius and height, described as cone
 * from [0, height] along the z axis
 */
class Cone : public Geometry {
	float radius, height;

public:
	/*
	 * Construct the cone with some radius and height
	 */
	Cone(float radius = 1, float height = 1);
	bool intersect(Ray &ray, DifferentialGeometry &dg) const override;
	BBox bound() const override;
	void refine(std::vector<Geometry*> &prims) override;
	/*
	 * Compute the surface area of the sphere
	 */
	float surface_area() const override;
};

#endif

