#include <cmath>
#include "material/specular_reflection.h"

SpecularReflection::SpecularReflection(const Colorf &reflection, Fresnel *fresnel)
	: BxDF(BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::SPECULAR)), reflection(reflection), fresnel(fresnel)
{}
Colorf SpecularReflection::operator()(const Vector&, const Vector&) const {
	return Colorf{0};
}
Colorf SpecularReflection::sample(const Vector &wo, Vector &wi, const std::array<float, 2>&, float &pdf_val) const {
	//Compute the perfect specular reflection direction since we're explicitly sampling the delta distribution
	wi = Vector{-wo.x, -wo.y, wo.z};
	pdf_val = 1;
	return (*fresnel)(cos_theta(wo)) * reflection / std::abs(cos_theta(wi));
}
float SpecularReflection::pdf(const Vector&, const Vector&) const {
	return 0;
}

