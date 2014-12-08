#include <array>
#include "volume/homogeneous_volume.h"

HomogeneousVolume::HomogeneousVolume(const Colorf &sig_a, const Colorf &sig_s, const Colorf &emit,
	float phase_asymmetry, const BBox &region)
	: sig_a(sig_a), sig_s(sig_s), emit(emit), phase_asymmetry(phase_asymmetry), region(region)
{}
BBox HomogeneousVolume::bound() const {
	return region;
}
bool HomogeneousVolume::intersect(const Ray &ray, std::array<float, 2> &t) const {
	return region.intersect(ray, &t[0], &t[1]);
}
Colorf HomogeneousVolume::absorption(const Point &p, const Vector&) const {
	return region.inside(p) ? sig_a : 0;
}
Colorf HomogeneousVolume::scattering(const Point &p, const Vector&) const {
	return region.inside(p) ? sig_s : 0;
}
Colorf HomogeneousVolume::attenuation(const Point &p, const Vector&) const {
	return region.inside(p) ? sig_a + sig_s : 0;
}
Colorf HomogeneousVolume::emission(const Point &p, const Vector&) const {
	return region.inside(p) ? emit : 0;
}
Colorf HomogeneousVolume::optical_thickness(const Ray &ray, float, float) const {
	std::array<float, 2> t;
	if (!region.intersect(ray, &t[0], &t[1])){
		return 0;
	}
	return ray(t[0]).distance(ray(t[1])) * (sig_a + sig_s);
}
float HomogeneousVolume::phase(const Point &p, const Vector &w_i, const Vector &w_o) const {
	return region.inside(p) ? phase_henyey_greenstein(w_i, w_o, phase_asymmetry) : 0;
}

