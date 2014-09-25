#include <cmath>
#include "linalg/vector.h"
#include "linalg/ray.h"
#include "geometry/plane.h"

bool Plane::intersect(Ray &ray, DifferentialGeometry &diff_geom){
	//If the ray is perpindicular to the normal there's no
	//way for it to hit the plane
	if (std::abs(ray.d.z) < 1e-8){
		return false;
	}
	//We're still treating the plane as infinite here so if it's not
	//perpindicular it definitely hits somewhere
	float t = -ray.o.z / ray.d.z;
	if (t <= ray.min_t || t >= ray.max_t){
		return false;
	}
	//It's in the range for the ray so now check if it's in range
	//for the finite plane
	Point hit = ray(t);
	if (hit.x >= -1 && hit.x <= 1 && hit.y >= -1 && hit.y <= 1){
		ray.max_t = t;
		diff_geom.point = hit;
		diff_geom.normal = Normal{0, 0, 1};
		if (ray.d.dot(diff_geom.normal) < 0){
			diff_geom.hit_side = HITSIDE::FRONT;
		}
		else {
			diff_geom.hit_side = HITSIDE::BACK;
		}
		//Compute parameterization of surface and various derivatives for texturing
		//Plane is parameterized by x and y coords
		diff_geom.u = (hit.x + 1) / 2;
		diff_geom.v = (hit.y + 1) / 2;
		//The change in x/y vs. u/v. Is this correct?
		diff_geom.dp_du = Vector{2, 0, 0};
		diff_geom.dp_dv = Vector{0, 2, 0};
		//Normal doesn't change over the plane so these are trivial
		diff_geom.dn_du = Normal{0, 0, 0};
		diff_geom.dn_dv = Normal{0, 0, 0};
		return true;
	}
	return false;
}
BBox Plane::bound() const {
	return BBox{Point{-1, -1, 0}, Point{1, 1, 0}};
}
void Plane::refine(std::vector<Geometry*> &prims){
	prims.push_back(this);
}

