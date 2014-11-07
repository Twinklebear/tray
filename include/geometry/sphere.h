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
	float surface_area() const;
	/*
	 * Sample a position on the geometry and return the point and normal
	 */
	Point sample(const std::array<float, 2> &u, Normal &normal) const;
	/*
	 * Sample the shape using the probability density of the solid angle from
	 * point p
	 * Return to 747 for sampling
	 */
	Point sample(const Point &p, const std::array<float, 2> &u, Normal &normal) const;
	/*
	 * Compute the pdf of sampling uniformly on the surface
	 */
	float pdf(const Point &p) const;
	/*
	 * Compute the pdf that the ray from p with direction w_i intersects the shape
	 */
	float pdf(const Point &p, const Vector &w_i) const;
};

#endif

