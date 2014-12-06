#include "monte_carlo/util.h"
#include "linalg/util.h"
#include "linalg/ray.h"
#include "linalg/vector.h"
#include "geometry/disk.h"

Disk::Disk(float radius, float inner_radius) : radius(radius), inner_radius(inner_radius){}
bool Disk::intersect(Ray &ray, DifferentialGeometry &dg) const {
	//We just intersect with the plane the disk lies in and then see if that point is on the disk
	//If the ray is perpindicular to the normal there's no
	//way for it to hit the plane
	if (std::abs(ray.d.z) < 1e-7){
		return false;
	}
	//We're still treating the plane as infinite here so if it's not
	//perpindicular it definitely hits somewhere
	float t = -ray.o.z / ray.d.z;
	if (t < ray.min_t || t > ray.max_t){
		return false;
	}
	//It's in the range for the ray so now check if it's in range
	//for the finite plane
	Point hit = ray(t);
	float dist_sqr = hit.x * hit.x + hit.y * hit.y;
	if (dist_sqr > radius * radius || dist_sqr < inner_radius * inner_radius){
		return false;
	}
	float phi = std::atan2(hit.y, hit.x);
	if (phi < 0){
		phi += TAU;
	}
	dg.point = hit;
	ray.max_t = t;

	dg.u = phi / TAU;
	dg.v = 1 - (std::sqrt(dist_sqr) - inner_radius) / (radius - inner_radius);
	float inv_z = 1 - dg.v > 0 ? 1.f / (1 - dg.v) : 0;
	dg.dp_du = Vector{-TAU * hit.y, TAU * hit.x, 0};
	dg.dp_dv = Vector{-hit.x * inv_z, -hit.y * inv_z, 0};
	dg.dp_dv *= (radius - inner_radius) / radius;
	//Normal doesn't change over the plane so these are trivial
	dg.normal = Normal{dg.dp_du.cross(dg.dp_dv).normalized()};
	dg.dn_du = Normal{0, 0, 0};
	dg.dn_dv = Normal{0, 0, 0};
	dg.geom = this;
	if (ray.d.dot(dg.normal) < 0){
		dg.hit_side = HITSIDE::FRONT;
	}
	else {
		dg.hit_side = HITSIDE::BACK;
	}
	return true;
}
BBox Disk::bound() const {
	return BBox{Point{-radius, -radius, 0}, Point{radius, radius, 0}};
}
void Disk::refine(std::vector<Geometry*> &prims){
	prims.push_back(this);
}
float Disk::surface_area() const {
	return PI * (radius * radius - inner_radius * inner_radius);
}
Point Disk::sample(const GeomSample &gs, Normal &normal) const {
	Point p;
	auto disk_pos = concentric_sample_disk(gs.u);
	p.x = disk_pos[0] * radius;
	p.y = disk_pos[1] * radius;
	clamp(p.x, inner_radius, radius);
	clamp(p.y, inner_radius, radius);
	normal = Normal{0, 0, 1};
	return p;
}
bool Disk::attach_light(const Transform&){
	return true;
}

