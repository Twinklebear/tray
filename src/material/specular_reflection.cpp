#include <cmath>
#include "material/specular_reflection.h"

SpecularReflection::SpecularReflection(const Colorf &reflection, Fresnel *fresnel)
	: BxDF(BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::SPECULAR)), reflection(reflection), fresnel(fresnel)
{}
Colorf SpecularReflection::operator()(const Vector&, const Vector&) const {
	return Colorf{0};
}
Colorf SpecularReflection::sample(const Vector &w_o, Vector &w_i, const std::array<float, 2>&, float &pdf_val) const {
	//Compute the perfect specular reflection direction since we're explicitly sampling the delta distribution
	w_i = Vector{-w_o.x, -w_o.y, w_o.z};
	pdf_val = 1;
	return (*fresnel)(cos_theta(w_o)) * reflection / std::abs(cos_theta(w_i));
}
float SpecularReflection::pdf(const Vector&, const Vector&) const {
	return 0;
}

