#ifndef DISK_H
#define DISK_H

#include "geometry.h"

/*
 * A disk with some inner and outer radius allowing it to
 * have a hole in the center placed at the origin with normal
 * along +z
 */
class Disk : public Geometry {
	float radius, inner_radius;

public:
	/*
	 * Construct the disk with some radius and inner radius
	 */
	Disk(float radius = 1, float inner_radius = 0);
	bool intersect(Ray &ray, DifferentialGeometry &dg) const override;
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

