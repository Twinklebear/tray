#include <cmath>
#include "linalg/vector.h"
#include "linalg/ray.h"
#include "geometry/plane.h"

bool Plane::intersect(Ray &r){
	float n_dot_r = r.d.dot(Vector{0, 0, 1});
	//If the ray is perpindicular to the normal there's no
	//way for it to hit the plane
	if (std::abs(n_dot_r) <= 0){
		return false;
	}
	//We're still treating the plane as infinite here so if it's not
	//perpindicular it definitely hits somewhere
	float t = -Vector{r.o}.dot(Vector{0, 0, 1}) / n_dot_r;
	if (t <= r.min_t || t >= r.max_t){
		return false;
	}
	//It's in the range for the ray so now check if it's in range
	//for the finite plane
	Point hit = r(t);
	return hit.x >= -1 && hit.x <= 1 && hit.y >= -1 && hit.y <= 1;
}

