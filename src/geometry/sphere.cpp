#include <iostream>
#include "linalg/util.h"
#include "linalg/ray.h"
#include "linalg/vector.h"
#include "geometry/sphere.h"

bool Sphere::intersect(Ray &ray, DifferentialGeometry &diff_geom){
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
	diff_geom.point = ray(t_hit);
	//For a unit sphere the normal is the same as the point hit
	diff_geom.normal = Normal{diff_geom.point};
	if (ray.d.dot(diff_geom.normal) < 0){
		diff_geom.hit_side = HITSIDE::FRONT;
	}
	else {
		diff_geom.hit_side = HITSIDE::BACK;
	}

	//Compute parameterization of surface and various derivatives for texturing
	//Sphere is parameterized by phi and theta coords
	float phi = std::atan2(diff_geom.point.x, diff_geom.point.y);
	float theta = std::acos(clamp(diff_geom.point.z, -1.f, 1.f));
	diff_geom.u = phi / TAU;
	diff_geom.v = theta / PI;

	//Compute derivatives for point vs. parameterization, using trig shortcuts to
	//skip cos/sin calls
	float inv_z = 1 / std::sqrt(diff_geom.point.x * diff_geom.point.x
		+ diff_geom.point.y * diff_geom.point.y);
	float cos_phi = diff_geom.point.x * inv_z;
	float sin_phi = diff_geom.point.y * inv_z;
	diff_geom.dp_du = Vector{-TAU * diff_geom.point.y, TAU * diff_geom.point.x, 0};
	diff_geom.dp_dv = PI * Vector{diff_geom.point.z * cos_phi, diff_geom.point.z * sin_phi,
		-std::sin(theta)};

	//Compute derivatives of normals using Weingarten eqns
	Vector ddp_duu = -TAU * TAU * Vector{diff_geom.point.x, diff_geom.point.y, 0};
	Vector ddp_duv = PI * TAU * diff_geom.point.z * Vector{-sin_phi, cos_phi, 0};
	Vector ddp_dvv = -PI * PI * Vector{diff_geom.point.x, diff_geom.point.y, diff_geom.point.z};

	float E = diff_geom.dp_du.dot(diff_geom.dp_du);
	float F = diff_geom.dp_du.dot(diff_geom.dp_dv);
	float G = diff_geom.dp_dv.dot(diff_geom.dp_dv);
	float e = diff_geom.normal.dot(ddp_duu);
	float f = diff_geom.normal.dot(ddp_duv);
	float g = diff_geom.normal.dot(ddp_dvv);
	float divisor = 1 / (E * G - F * F);
	diff_geom.dn_du = Normal{(f * F - e * G) * divisor * diff_geom.dp_du
		+ (e * F - f * E) * divisor * diff_geom.dp_dv};
	diff_geom.dn_dv = Normal{(g * F - f * G) * divisor * diff_geom.dp_du
		+ (f * F - g * E) * divisor * diff_geom.dp_dv};
	return true;
}
BBox Sphere::bound() const {
	return BBox{Point{-1, -1, -1}, Point{1, 1, 1}};
}
void Sphere::refine(std::vector<Geometry*> &prims){
	prims.push_back(this);
}

