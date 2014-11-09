#include <cmath>
#include "material/fresnel.h"
#include "material/specular_transmission.h"

SpecularTransmission::SpecularTransmission(const Colorf &transmission, FresnelDielectric *fresnel)
	: BxDF(BxDFTYPE(BxDFTYPE::TRANSMISSION | BxDFTYPE::SPECULAR)), transmission(transmission), fresnel(fresnel)
{}
Colorf SpecularTransmission::operator()(const Vector&, const Vector&) const {
	return Colorf{0};
}
Colorf SpecularTransmission::sample(const Vector &w_o, Vector &w_i, const std::array<float, 2>&, float &pdf_val) const {
	//Determine the side of the material we're incident on and pick indices of refraction accordingly
	bool entering = cos_theta(w_o) > 0;
	float ei = entering ? fresnel->eta_i : fresnel->eta_t;
	float et = entering ? fresnel->eta_t : fresnel->eta_i;
	float sin_i2 = sin_theta2(w_o);
	float eta = ei / et;
	float sin_t2 = eta * eta * sin_i2;
	//If total internal reflection, nothing is transmitted
	if (sin_t2 >= 1){
		return Colorf{0};
	}
	//Compute the transmitted ray direction
	float cos_t = std::sqrt(std::max(0.f, 1.f - sin_t2));
	cos_t = entering ? -cos_t : cos_t;
	w_i = Vector{eta * -w_o.x, eta * -w_o.y, cos_t};
	pdf_val = 1;
	return (Colorf{1} - (*fresnel)(cos_theta(w_o))) * transmission / std::abs(cos_theta(w_i));
}
float SpecularTransmission::pdf(const Vector&, const Vector&) const {
	return 0;
}

