#include "volume/volume.h"

float phase_rayleigh(const Vector &w_i, const Vector &w_o){
	float cos_t = w_i.dot(w_o);
	return 3.f / (16 * PI) * (1 + cos_t * cos_t);
}
float phase_mie_hazy(const Vector &w_i, const Vector &w_o){
	float cos_t = w_i.dot(w_o);
	return (0.5 + 4.5 * std::pow(0.5 * (1.f + cos_t), 8.f)) / (4 * PI);
}
float phase_mie_murky(const Vector &w_i, const Vector &w_o){
	float cos_t = w_i.dot(w_o);
	return (0.5 + 16.5 * std::pow(0.5 * (1.f + cos_t), 32.f)) / (4 * PI);
}
float phase_henyey_greenstein(const Vector &w_i, const Vector &w_o, float g){
	float cos_t = w_i.dot(w_o);
	return 1.f / (4 * PI) * (1 - g * g) / std::pow(1 + g * g - 2 * g * cos_t, 1.5f);
}
float phase_schlick(const Vector &w_i, const Vector &w_o, float g){
	float alpha = 1.5;
	float k = alpha * g + (1 - alpha) * std::pow(g, 3.f);
	float k_cos_t = k * w_i.dot(w_o);
	return 1.f / (4 * PI) * (1 - k * k) / ((1 - k_cos_t) * (1 - k_cos_t));
}

Colorf Volume::attenuation(const Point &p, const Vector &v) const {
	return absorption(p, v) + scattering(p, v);
}

