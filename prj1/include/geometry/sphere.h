#ifndef SPHERE_H
#define SPHERE_H

#include "linalg/point.h"
#include "geometry.h"

class Sphere : public Geometry {
	Point center;
	float radius;

public:
	Sphere(const Point &center, float radius);
	bool intersect(Ray &r) override;
};

#endif

