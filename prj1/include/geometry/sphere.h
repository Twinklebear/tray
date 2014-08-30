#ifndef SPHERE_H
#define SPHERE_H

#include "linalg/point.h"
#include "geometry.h"

/*
 * A simple unit sphere centered at the origin
 */
class Sphere : public Geometry {
	bool intersect(Ray &r) override;
};

#endif

