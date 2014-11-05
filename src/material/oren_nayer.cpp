#include <cmath>
#include "linalg/util.h"
#include "material/oren_nayer.h"

OrenNayer::OrenNayer(const Colorf &reflectance, float sigma) : BxDF(BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::DIFFUSE)),
	reflectance(reflectance)
{
	//We just need sigma^2 in radians
	sigma = radians(sigma);
	sigma *= sigma;
	a = 1.f - sigma / (2.f * (sigma + 0.33f));
	b = 0.45f * sigma / (sigma + 0.09f);
}
Colorf OrenNayer::operator()(const Vector &wo, const Vector &wi) const {
	float alpha = std::max(cos_theta(wi), cos_theta(wo));
	float beta = std::min(cos_theta(wi), cos_theta(wo));
	float max_cos = 0;
	if (sin_theta(wi) > 1e-4 && sin_theta(wo) > 1e-4){
		max_cos = std::max(0.f, cos_phi(wi) * cos_phi(wo) + sin_phi(wi) * sin_phi(wo));
	}
	return reflectance * INV_PI * (a + b * max_cos * std::sin(alpha) * std::tan(beta));
}

