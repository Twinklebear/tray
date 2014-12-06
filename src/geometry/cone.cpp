#include "monte_carlo/util.h"
#include "linalg/util.h"
#include "linalg/ray.h"
#include "linalg/vector.h"
#include "geometry/cone.h"

Cone::Cone(float radius, float height) : radius(radius), height(height){}
bool Cone::intersect(Ray &ray, DifferentialGeometry &dg) const {
	//Compute the cone quadratic coefficients
	float k = radius / height;
	k *= k;
	float a = ray.d.x * ray.d.x + ray.d.y * ray.d.y - k * ray.d.z * ray.d.z;
	float b = 2 * (ray.d.x * ray.o.x + ray.d.y * ray.o.y - k * ray.d.z * (ray.o.z - height));
	float c = ray.o.x * ray.o.x + ray.o.y * ray.o.y - k * (ray.o.z - height) * (ray.o.z - height);
	//Solve quadratic equation for t values
	//If no solutions exist the ray doesn't intersect the cone
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
	Point point = ray(t_hit);
	float phi = std::atan2(point.y, point.x);
	if (phi < 0){
		phi += TAU;
	}
	//The test is against an infinitely long cone, so check that the hit is in range
	if (point.z < 0 || point.z > height){
		if (t_hit == t[1]){
			return false;
		}
		t_hit = t[1];
		if (t_hit > ray.max_t){
			return false;
		}
		point = ray(t_hit);
		phi = std::atan2(point.y, point.x);
		if (phi < 0){
			phi += TAU;
		}
		if (point.z < 0 || point.z > height){
			return false;
		}
	}
	dg.point = point;
	ray.max_t = t_hit;

	dg.u = phi / TAU;
	dg.v = dg.point.z / height;
	dg.dp_du = Vector{-TAU * point.y, TAU * point.x, 0};
	dg.dp_dv = Vector{-point.x / (1 - dg.v), -point.y / (1 - dg.v), height};

	dg.normal = Normal{dg.dp_du.cross(dg.dp_dv).normalized()};
	//Compute derivatives of normals using Weingarten eqns
	Vector ddp_duu = -TAU * TAU * Vector{point.x, point.y, 0};
	Vector ddp_duv = TAU / (1 - dg.v) * Vector{point.y, -point.x, 0};
	Vector ddp_dvv{0};

	float E = dg.dp_du.dot(dg.dp_du);
	float F = dg.dp_du.dot(dg.dp_dv);
	float G = dg.dp_dv.dot(dg.dp_dv);
	float e = dg.normal.dot(ddp_duu);
	float f = dg.normal.dot(ddp_duv);
	float g = dg.normal.dot(ddp_dvv);
	float divisor = 1 / (E * G - F * F);
	dg.dn_du = Normal{(f * F - e * G) * divisor * dg.dp_du
		+ (e * F - f * E) * divisor * dg.dp_dv};
	dg.dn_dv = Normal{(g * F - f * G) * divisor * dg.dp_du
		+ (f * F - g * E) * divisor * dg.dp_dv};
	dg.geom = this;
	if (ray.d.dot(dg.normal) < 0){
		dg.hit_side = HITSIDE::FRONT;
	}
	else {
		dg.hit_side = HITSIDE::BACK;
	}
	return true;
}
BBox Cone::bound() const {
	return BBox{Point{-radius, -radius, 0}, Point{radius, radius, height}};
}
void Cone::refine(std::vector<Geometry*> &prims){
	prims.push_back(this);
}
float Cone::surface_area() const {
	return PI * radius * std::sqrt(height * height + radius * radius);
}

