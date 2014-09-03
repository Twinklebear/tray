#ifndef SPHERE_H
#define SPHERE_H

#include "linalg/point.h"
#include "geometry.h"

/*
 * A simple unit sphere centered at the origin
 */
class Sphere : public Geometry {
public:
	bool intersect(Ray &ray, HitInfo &hitinfo) override;
};

#endif

