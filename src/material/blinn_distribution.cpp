#include <cmath>
#include "material/bxdf.h"
#include "material/blinn_distribution.h"

BlinnDistribution::BlinnDistribution(float exponent) : exponent(exponent){}
float BlinnDistribution::operator()(const Vector &w_h) const {
	return (exponent + 2) / INV_TAU * std::pow(std::abs(BxDF::cos_theta(w_h)), exponent);
}
void BlinnDistribution::sample(const Vector &w_o, Vector &w_i, const std::array<float, 2> &u, float &pdf_val) const {
	//Sample a direction on the hemisphere for the half-vector
	float cos_theta = std::pow(u[0], 1 / (exponent + 1));
	float sin_theta = std::sqrt(std::max(0.f, 1 - cos_theta * cos_theta));
	float phi = TAU * u[1];
	Vector w_h = spherical_dir(sin_theta, cos_theta, phi);
	if (!BxDF::same_hemisphere(w_o, w_h)){
		w_h = -w_h;
	}
	//The sampled incident direction is the outgoing direction reflected about the half-vector
	w_i = -w_o + 2 * w_o.dot(w_h) * w_h;
	pdf_val = ((exponent + 1) * std::pow(cos_theta, exponent)) / (TAU * 4 * w_o.dot(w_h));
	if (w_o.dot(w_h) <= 0){
		pdf_val = 0;
	}
}
float BlinnDistribution::pdf(const Vector &w_o, const Vector &w_i) const {
	Vector w_h = (w_o + w_i).normalized();
	float cos_theta = std::abs(BxDF::cos_theta(w_h));
	float pdf_val = ((exponent + 1) * std::pow(cos_theta, exponent)) / (TAU * 4 * w_o.dot(w_h));
	if (w_o.dot(w_h) <= 0){
		pdf_val = 0;
	}
	return pdf_val;
}

