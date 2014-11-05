#include <cassert>
#include <cmath>
#include "film/color.h"
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
	Colorf c;
	for (const auto &s : samples){
		Vector wi;
		float pdf_val = 0;
		Colorf f = sample(wo, wi, s, pdf_val);
		if (pdf_val > 0){
			c += f * std::abs(cos_theta(wi)) / pdf_val;
		}
	}
	return c / samples.size();
}
Colorf BxDF::rho_hh(const std::vector<std::array<float, 2>> &samples_a,
	const std::vector<std::array<float, 2>> &samples_b) const
{
	assert(samples_a.size() == samples_b.size());
	Colorf c;
	for (auto a = samples_a.begin(), b = samples_b.begin(); a != samples_a.end(); ++a, ++b){
		Vector wo = uniform_sample_hemisphere(*a);
		float pdf_o = uniform_hemisphere_pdf();
		Vector wi;
		float pdf_i = 0;
		Colorf f = sample(wo, wi, *b, pdf_i);
		if (pdf_i > 0){
			c += f * std::abs(cos_theta(wo)) * std::abs(cos_theta(wi)) / (pdf_o * pdf_i);
		}
	}
	return c / (PI * samples_a.size());
}
float BxDF::pdf(const Vector &wo, const Vector &wi) const {
	//If wo and wi are not in the same hemisphere there's no chance of sampling
	return same_hemisphere(wo, wi) ? std::abs(cos_theta(wi)) * INV_PI : 0;
}

