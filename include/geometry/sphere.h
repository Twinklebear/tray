#ifndef SPHERE_H
#define SPHERE_H

#include <array>
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
	float surface_area() const override;
	/*
	 * Sample a position on the geometry and return the point and normal
	 */
	Point sample(const GeomSample &gs, Normal &normal) const override;
	/*
	 * Sample the shape using the probability density of the solid angle from
	 * point p to the point on the surface
	 */
	Point sample(const Point &p, const GeomSample &gs, Normal &normal) const override;
	/*
	 * Compute the pdf that the ray from p with direction w_i intersects the shape
	 */
	float pdf(const Point &p, const Vector &w_i) const override;
	bool attach_light(const Transform &to_world) override;
};

#endif

