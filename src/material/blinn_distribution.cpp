#include <cmath>
#include "material/bxdf.h"
#include "material/blinn_distribution.h"

BlinnDistribution::BlinnDistribution(float exponent) : exponent(exponent){}
float BlinnDistribution::operator()(const Vector &w_h) const {
	return (exponent + 2) / INV_TAU * std::pow(std::abs(BxDF::cos_theta(w_h)), exponent);
}

