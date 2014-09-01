#ifndef PLANE_H
#define PLANE_H

#include "geometry.h"

/*
 * A plane centered at the origin spanning
 * [-1, -1] to [1, 1] with normal of {0, 0, 1}
 */
class Plane : public Geometry {
public:
	bool intersect(Ray &r) override;
};

#endif

