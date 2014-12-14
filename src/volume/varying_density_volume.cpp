#include <array>
#include "volume/varying_density_volume.h"

VaryingDensityVolume::VaryingDensityVolume(const Colorf &sig_a, const Colorf &sig_s, const Colorf &emit, float phase_asymmetry)
	: sig_a(sig_a), sig_s(sig_s), emit(emit), phase_asymmetry(phase_asymmetry)
{}
Colorf VaryingDensityVolume::absorption(const Point &p, const Vector&) const {
	return density(p) * sig_a;
}
Colorf VaryingDensityVolume::scattering(const Point &p, const Vector&) const {
	return density(p) * sig_s;
}
Colorf VaryingDensityVolume::attenuation(const Point &p, const Vector&) const {
	return density(p) * (sig_a + sig_s);
}
Colorf VaryingDensityVolume::emission(const Point &p, const Vector&) const {
	return density(p) * emit;
}
Colorf VaryingDensityVolume::optical_thickness(const Ray &ray, float step, float offset) const {
	float length = ray.d.length();
	if (length == 0){
		return 0;
	}
	std::array<float, 2> t;
	Ray r{ray.o, ray.d / length, r.min_t * length, r.max_t * length};
	if (!intersect(r, t)){
		return 0;
	}
	Colorf tau;
	for (float i = t[0] + offset * step; i < t[1]; i += step){
		tau += attenuation(r(i), -r.d);
	}
	return tau * step;
}
float VaryingDensityVolume::phase(const Point&, const Vector &w_i, const Vector &w_o) const {
	return phase_henyey_greenstein(w_i, w_o, phase_asymmetry);
}

