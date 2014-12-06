#include "monte_carlo/util.h"
#include "linalg/util.h"
#include "linalg/ray.h"
#include "linalg/vector.h"
#include "geometry/geometry.h"
#include "geometry/cylinder.h"

Cylinder::Cylinder(float radius, float height) : radius(radius), height(height){}
bool Cylinder::intersect(Ray &ray, DifferentialGeometry &dg) const {
	//Notes: in PBRT speak my zmin = 0, zmax = height, phimax = 2pi
	float a = ray.d.x * ray.d.x + ray.d.y * ray.d.y;
    float b = 2 * (ray.d.x * ray.o.x + ray.d.y * ray.o.y);
    float c = ray.o.x * ray.o.x + ray.o.y * ray.o.y - radius * radius;
	//Solve quadratic equation for t values
	//If no solutions exist the ray doesn't intersect the cylinder
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
	//The hit test is against an infinitely long cylinder so now check that it's actually in
	//the [0, height] range
	if (point.z < 0 || point.z > height){
		if (t_hit == t[1]){
			return false;
		}
		t_hit = t[1];
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
	dg.dp_du = Vector{-TAU * dg.point.y, TAU * dg.point.x, 0};
	dg.dp_dv = Vector{0, 0, height};
	
	dg.normal = Normal{dg.point.x, dg.point.y, 0}.normalized();
	//Compute derivatives of normals using Weingarten eqns
	Vector ddp_duu = -TAU * TAU * Vector{dg.point.x, dg.point.y, 0};
	Vector ddp_duv{0}, ddp_dvv{0};

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
BBox Cylinder::bound() const {
	return BBox{Point{-radius, -radius, 0}, Point{radius, radius, height}};
}
void Cylinder::refine(std::vector<Geometry*> &prims){
	prims.push_back(this);
}
float Cylinder::surface_area() const {
	return height * TAU * radius;
}
Point Cylinder::sample(const GeomSample &gs, Normal &normal) const {
	float z = lerp(gs.u[0], 0.f, height);
	float phi = gs.u[1] * TAU;
	Point p{radius * std::cos(phi), radius * std::sin(phi), z};
	normal = Normal{p.x, p.y, 0}.normalized();
	return p;
}
bool Cylinder::attach_light(const Transform&){
	return true;
}

