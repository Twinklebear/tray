#include "material/bxdf.h"

BxDF::BxDF(BxDFTYPE t) : type(t){}
bool BxDF::matches(BxDFTYPE flags) const {
	return (type & flags) == type;
}
Colorf BxDF::sample(const Vector &wo, Vector &wi, const std::array<float, 2> &samples, float &pdf) const {
	return Colorf{0};
}
Colorf BxDF::rho_hd(const Vector &wo, const std::vector<float> &samples) const {
	return Colorf{0};
}
Colorf BxDF::rho_hh(const std::vector<float> &samples_a, const std::vector<float> &samples_b) const {
	return Colorf{0};
}
float BxDF::pdf(const Vector &wo, const Vector &wi) const {
	return 0;
}

