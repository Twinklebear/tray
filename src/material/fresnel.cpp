#include <cmath>
#include "linalg/util.h"
#include "material/fresnel.h"

Colorf fresnel_dielectric(float cos_i, float cos_t, float eta_i, float eta_t){
	Colorf r_par = (eta_t * cos_i - eta_i * cos_t) / (eta_t * cos_i + eta_i * cos_t);
	Colorf r_perp = (eta_i * cos_i - eta_t * cos_t) / (eta_i * cos_i + eta_t * cos_t);
	return 0.5 * (r_par * r_par + r_perp * r_perp);
}
Colorf fresnel_conductor(float cos_i, const Colorf &eta, const Colorf &k){
	Colorf a = (eta * eta + k * k) * cos_i * cos_i;
	Colorf r_par = (a - 2 * eta * cos_i + 1) / (a + 2 * eta * cos_i + 1);
	a = eta * eta + k * k;
	Colorf r_perp = (a - 2 * eta * cos_i + cos_i * cos_i) / (a + 2 * eta * cos_i + cos_i * cos_i);
	//These are actually r_par^2 and r_perp^2, so don't square here
	return 0.5 * (r_par + r_perp);
}

FresnelDielectric::FresnelDielectric(float eta_i, float eta_t)
	: eta_i(eta_i), eta_t(eta_t)
{}
Colorf FresnelDielectric::operator()(float cos_i) const {
	//We need to find out which side of the material we're incident on so
	//we can pass the correct indices of refraction
	cos_i = clamp(cos_i, -1.f, 1.f);
	float ei = cos_i > 0 ? eta_i : eta_t;
	float et = cos_i > 0 ? eta_t : eta_i;
	float sin_t = ei / et * std::sqrt(std::max(0.f, 1.f - cos_i * cos_i));
	//Handle total internal reflection
	if (sin_t >= 1){
		return Colorf{1};
	}
	float cos_t = std::sqrt(std::max(0.f, 1.f - sin_t * sin_t));
	return fresnel_dielectric(cos_i, cos_t, ei, et);
}
FresnelConductor::FresnelConductor(const Colorf &eta, const Colorf &k) : eta(eta), k(k){}
Colorf FresnelConductor::operator()(float cos_i) const {
	return fresnel_conductor(std::abs(cos_i), eta, k);
}

Colorf FresnelNoOp::operator()(float cos_i) const {
	return Colorf{1};
}

