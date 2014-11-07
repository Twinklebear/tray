#ifndef SPHERE_H
#define SPHERE_H

#include "linalg/point.h"
#include "geometry.h"

/*
 * A simple unit sphere centered at the origin
 */
class Sphere : public Geometry {
	float radius;

public:
	/*
	 * Construct the sphere with some radius
	 */
	Sphere(float radius = 1);
	bool intersect(Ray &ray, DifferentialGeometry &diff_geom) const override;
	BBox bound() const override;
	void refine(std::vector<Geometry*> &prims) override;
	/*
	 * Compute the surface area of the sphere
	 */
	float surface_area() const;
};

#endif

