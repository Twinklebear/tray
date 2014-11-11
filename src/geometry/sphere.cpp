#include <iostream>
#include "monte_carlo/util.h"
#include "linalg/util.h"
#include "linalg/ray.h"
#include "linalg/vector.h"
#include "geometry/sphere.h"

Sphere::Sphere(float radius) : radius(radius){}
bool Sphere::intersect(Ray &ray, DifferentialGeometry &diff_geom) const {
	//Compute quadratic sphere coefficients
	Vector ray_orig{ray.o};
	float a = ray.d.length_sqr();
	float b = 2 * ray.d.dot(ray_orig);
	float c = ray_orig.length_sqr() - radius * radius;
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
	diff_geom.geom_normal = diff_geom.normal;
	if (ray.d.dot(diff_geom.normal) < 0){
		diff_geom.hit_side = HITSIDE::FRONT;
	}
	else {
		diff_geom.hit_side = HITSIDE::BACK;
	}

	//Compute parameterization of surface and various derivatives for texturing
	//Sphere is parameterized by phi and theta coords
	float phi = std::atan2(diff_geom.point.x, diff_geom.point.y);
	if (phi < 0){
		phi += TAU;
	}
	float theta = std::acos(clamp(diff_geom.point.z / radius, -1.f, 1.f));
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
		-radius * std::sin(theta)};

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
	return BBox{Point{-radius, -radius, -radius},
		Point{radius, radius, radius}};
}
void Sphere::refine(std::vector<Geometry*> &prims){
	prims.push_back(this);
}
float Sphere::surface_area() const {
	return 2 * TAU * radius;
}
Point Sphere::sample(const GeomSample &gs, Normal &normal) const {
	Point p = Point{0, 0, 0} + radius * uniform_sample_sphere(gs.u);
	normal = Normal{p.x, p.y, p.z}.normalized();
	return p;
}
Point Sphere::sample(const Point &p, const GeomSample &gs, Normal &normal) const {
	//Compute coordinate system for sampling the sphere where z is the vector from the center to the point
	Vector w_z = Vector{-p}.normalized();
	Vector w_x, w_y;
	coordinate_system(w_z, w_x, w_y);
	//If we're inside the sphere we can just sample it uniformly
	if (p.distance_sqr(Point{0, 0, 0}) - radius * radius < 1e-4){
		return sample(gs, normal);
	}
	//Compute angle of cone that we see of the sphere
	float cos_theta = std::sqrt(std::max(0.f, 1 - radius * radius / p.distance_sqr(Point{0, 0, 0})));
	DifferentialGeometry dg;
	Ray ray{p, uniform_sample_cone(gs.u, cos_theta, w_x, w_y, w_z), 0.001};
	//We might not hit the sphere due to some numerical errors, so make sure it does hit
	if (!intersect(ray, dg)){
		ray.max_t = ray.d.normalized().dot(Point{0, 0, 0} - p);
	}
	Point ps = ray(ray.max_t);
	normal = Normal{ps}.normalized();
	return ps;
}
float Sphere::pdf(const Point &p, const Vector &w_i) const {
	//If we're in the sphere compute the weight inside but defined over the solid angle
	if (p.distance_sqr(Point{0, 0, 0}) - radius * radius < 1e-4){
		return Geometry::pdf(p, w_i);
	}
	float cos_theta = std::sqrt(std::max(0.f, 1 - radius * radius / p.distance_sqr(Point{0, 0, 0})));
	return uniform_cone_pdf(cos_theta);
}
bool Sphere::attach_light(const Transform&){
	return true;
}

