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
Colorf OrenNayer::operator()(const Vector &w_o, const Vector &w_i) const {
	float alpha = std::max(cos_theta(w_i), cos_theta(w_o));
	float beta = std::min(cos_theta(w_i), cos_theta(w_o));
	float max_cos = 0;
	if (sin_theta(w_i) > 1e-4 && sin_theta(w_o) > 1e-4){
		max_cos = std::max(0.f, cos_phi(w_i) * cos_phi(w_o) + sin_phi(w_i) * sin_phi(w_o));
	}
	return reflectance * INV_PI * (a + b * max_cos * std::sin(alpha) * std::tan(beta));
}

