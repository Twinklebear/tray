#include <cmath>
#include "monte_carlo/util.h"
#include "material/bxdf.h"

BxDF::BxDF(BxDFTYPE t) : type(t){}
bool BxDF::matches(BxDFTYPE flags) const {
	return (type & flags) == type;
}
Colorf BxDF::sample(const Vector &wo, Vector &wi, const std::array<float, 2> &samples, float &pdf_val) const {
	wi = cos_sample_hemisphere(samples);
	//We may need to flip the sample to be in the same hemisphere as wo
	if (wo.z < 0){
		wi.z *= -1;
	}
	pdf_val = pdf(wo, wi);
	return (*this)(wo, wi);
}
Colorf BxDF::rho_hd(const Vector &wo, const std::vector<std::array<float, 2>> &samples) const {
	return Colorf{0};
}
Colorf BxDF::rho_hh(const std::vector<std::array<float, 2>> &samples_a,
	const std::vector<std::array<float, 2>> &samples_b) const
{
	return Colorf{0};
}
float BxDF::pdf(const Vector &wo, const Vector &wi) const {
	//If wo and wi are not in the same hemisphere there's no chance of sampling
	return wo.z * wi.z > 0 ? std::abs(cos_theta(wi)) * INV_PI : 0;
}

