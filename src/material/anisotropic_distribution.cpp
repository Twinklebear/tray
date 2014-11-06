#include <cmath>
#include "material/bxdf.h"
#include "material/anisotropic_distribution.h"

AnisotropicDistribution::AnisotropicDistribution(float exp_x, float exp_y) : exp_x(exp_x), exp_y(exp_y){}
float AnisotropicDistribution::operator()(const Vector &w_h) const {
	float cos_th = std::abs(BxDF::cos_theta(w_h));
	float denom = 1 - cos_th * cos_th;
	if (denom == 0){
		return 0;
	}
	float exponent = (exp_x * w_h.x * w_h.x + exp_y * w_h.y * w_h.y) / denom;
	return std::sqrt((exp_x + 2) * (exp_y + 2)) * INV_TAU * std::pow(cos_th, exponent);
}

