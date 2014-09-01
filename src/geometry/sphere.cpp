#include <iostream>
#include "linalg/util.h"
#include "linalg/ray.h"
#include "linalg/vector.h"
#include "geometry/sphere.h"

bool Sphere::intersect(Ray &ray){
	//Compute quadratic sphere coefficients
	Vector ray_orig{ray.o};
	float a = ray.d.length_sqr();
	float b = 2 * ray.d.dot(ray_orig);
	float c = ray_orig.length_sqr() - 1;
	//Solve quadratic equation for t values
	//If no solutions exist the ray doesn't intersect the sphere
	float t[2];
	if (!solve_quadratic(a, b, c, t[0], t[1])){
		return false;
	}
	//Early out, if t[0] (min) is > max or t[1] (max) is < min then both
	//our hits are outside of the region we're testing
	if (t[0] > ray.max_t || t[1] < ray.min_t){
		return false;
	}
	//Find the t value that is within the region we care about, or return if neither is
	float t_hit = t[0];
	if (t_hit < ray.min_t){
		t_hit = t[1];
		if (t_hit > ray.max_t){
			return false;
		}
	}
	ray.max_t = t_hit;
	return true;
}

