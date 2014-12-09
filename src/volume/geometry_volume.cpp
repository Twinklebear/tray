#include <cassert>
#include <array>
#include "geometry/differential_geometry.h"
#include "volume/geometry_volume.h"

GeometryVolume::GeometryVolume(const Colorf &sig_a, const Colorf &sig_s, const Colorf &emit,
	float phase_asymmetry, const Geometry *geometry)
	: sig_a(sig_a), sig_s(sig_s), emit(emit), phase_asymmetry(phase_asymmetry), geometry(geometry)
{}
BBox GeometryVolume::bound() const {
	return geometry->bound();
}
bool GeometryVolume::intersect(const Ray &r, std::array<float, 2> &t) const {
	//We need to shoot the ray to hit the object to find t[0], then re-shoot it out
	//the other side to find t[1]
	Ray ray = r;
	DifferentialGeometry dg;
	if (!geometry->intersect(ray, dg)){
		return false;
	}
	t[0] = ray.max_t;
	//Setting ray.min_t seems to give too thin of volumes?
	ray.min_t = ray.max_t + 0.0001;
	ray.max_t = std::numeric_limits<float>::infinity();
	if (!geometry->intersect(ray, dg)){
		return false;
	}
	t[1] = ray.max_t;
	return true;
}
Colorf GeometryVolume::absorption(const Point &p, const Vector&) const {
	return inside(p) ? sig_a : 0;
}
Colorf GeometryVolume::scattering(const Point &p, const Vector&) const {
	return inside(p) ? sig_s : 0;
}
Colorf GeometryVolume::attenuation(const Point &p, const Vector&) const {
	return inside(p) ? sig_a + sig_s : 0;
}
Colorf GeometryVolume::emission(const Point &p, const Vector&) const {
	return inside(p) ? emit : 0;
}
Colorf GeometryVolume::optical_thickness(const Ray &ray, float, float) const {
	std::array<float, 2> t;
	if (!intersect(ray, t)){
		return 0;
	}
	return ray(t[0]).distance(ray(t[1])) * (sig_a + sig_s);
}
float GeometryVolume::phase(const Point &p, const Vector &w_i, const Vector &w_o) const {
	return inside(p) ? phase_henyey_greenstein(w_i, w_o, phase_asymmetry) : 0;
}
bool GeometryVolume::inside(const Point &p) const {
	std::array<DifferentialGeometry, 2> dg;
	std::array<RayDifferential, 2> ray{RayDifferential{p, Vector{1, 0, 0}},
		RayDifferential{p, Vector{-1, 0, 0}}
	};
	return geometry->intersect(ray[0], dg[0]) && geometry->intersect(ray[1], dg[1]);
}

