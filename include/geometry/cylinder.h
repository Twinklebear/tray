#ifndef CYLINDER_H
#define CYLINDER_H

#include "geometry.h"

/*
 * A cylinder with some radius and height
 */
class Cylinder : public Geometry {
	float radius, height;

public:
	/*
	 * Construct the cylinder with some radius and height
	 */
	Cylinder(float radius = 1, float height = 1);
	bool intersect(Ray &ray, DifferentialGeometry &diff_geom) const override;
	BBox bound() const override;
	void refine(std::vector<Geometry*> &prims) override;
	/*
	 * Compute the surface area of the sphere
	 */
	float surface_area() const override;
	/*
	 * Sample a position on the geometry and return the point and normal
	 */
	Point sample(const GeomSample &gs, Normal &normal) const override;
	bool attach_light(const Transform &to_world) override;
};

#endif

