#include <cmath>
#include "volume/exponential_volume.h"

ExponentialVolume::ExponentialVolume(const Colorf &sig_a, const Colorf &sig_s, const Colorf &emit, float phase_asymmetry,
	const BBox &region, float a, float b, const Vector &up)
	: VaryingDensityVolume(sig_a, sig_s, emit, phase_asymmetry), a(a), b(b), up(up), region(region)
{}
BBox ExponentialVolume::bound() const {
	return region;
}
bool ExponentialVolume::intersect(const Ray &ray, std::array<float, 2> &t) const {
	return region.intersect(ray, &t[0], &t[1]);
}
float ExponentialVolume::density(const Point &p) const {
	if (!region.inside(p)){
		return 0;
	}
	float h = up.dot(p - region.min);
	return a * std::exp(-b * h);
}

