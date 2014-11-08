#include <cmath>
#include "material/torrance_sparrow.h"


TorranceSparrow::TorranceSparrow(const Colorf &reflectance, Fresnel *fresnel, MicrofacetDistribution *distribution)
	: BxDF(BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::GLOSSY)), reflectance(reflectance),
	fresnel(fresnel), distribution(distribution)
{}

Colorf TorranceSparrow::operator()(const Vector &w_o, const Vector &w_i) const {
	float cos_to = std::abs(cos_theta(w_o));
	float cos_ti = std::abs(cos_theta(w_i));
	if (cos_to == 0 || cos_ti == 0){
		return Colorf{0};
	}
	Vector w_h = w_i + w_o;
	if (w_h.length_sqr() == 0){
		return Colorf{0};
	}
	w_h = w_h.normalized();
	float cos_th = w_i.dot(w_h);
	return reflectance * (*distribution)(w_h) * distribution->geom_atten(w_o, w_i, w_h) * (*fresnel)(cos_th)
		/ (4 * cos_ti * cos_to);
}
Colorf TorranceSparrow::sample(const Vector &w_o, Vector &w_i, const std::array<float, 2> &samples, float &pdf_val) const {
	distribution->sample(w_o, w_i, samples, pdf_val);
	if (!same_hemisphere(w_o, w_i)){
		return Colorf{0};
	}
	return (*this)(w_o, w_i);
}
float TorranceSparrow::pdf(const Vector &w_o, const Vector &w_i) const {
	if (!same_hemisphere(w_o, w_i)){
		return 0;
	}
	return distribution->pdf(w_o, w_i);
}

