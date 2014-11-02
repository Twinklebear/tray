#include <cmath>
#include "material/fresnel.h"
#include "material/specular_transmission.h"

SpecularTransmission::SpecularTransmission(const Colorf &transmission, float eta_i, float eta_t)
	: BxDF(BxDFTYPE(BxDFTYPE::TRANSMISSION | BxDFTYPE::SPECULAR)), transmission(transmission),
	eta_i(eta_i), eta_t(eta_t), fresnel(eta_t, eta_i)
{}
Colorf SpecularTransmission::operator()(const Vector&, const Vector&) const {
	return Colorf{0};
}
Colorf SpecularTransmission::sample(const Vector &wo, Vector &wi, const std::array<float, 2>&, float &pdf_val) const {
	//Determine the side of the material we're incident on and pick indices of refraction accordingly
	bool entering = cos_theta(wo) > 0;
	float ei = entering ? eta_i : eta_t;
	float et = entering ? eta_t : eta_i;
	float sin_i2 = sin_theta2(wo);
	float eta = ei / et;
	float sin_t2 = eta * eta * sin_i2;
	//If total internal reflection, nothing is transmitted
	if (sin_t2 >= 1){
		return Colorf{0};
	}
	//Compute the transmitted ray direction
	float cos_t = std::sqrt(std::max(0.f, 1.f - sin_t2));
	cos_t = entering ? -cos_t : cos_t;
	wi = Vector{eta * -wo.x, eta * -wo.y, cos_t};
	pdf_val = 1;
	return (Colorf{1} - fresnel(cos_theta(wo))) * transmission / std::abs(cos_theta(wi));
}
float SpecularTransmission::pdf(const Vector&, const Vector&) const {
	return 0;
}

