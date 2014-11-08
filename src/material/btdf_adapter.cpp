#include "material/btdf_adapter.h"

BTDFAdapter::BTDFAdapter(BxDF *b) : BxDF(BxDFTYPE(b->type ^ (BxDFTYPE::REFLECTION | BxDFTYPE::TRANSMISSION))), bxdf(b){}
Colorf BTDFAdapter::operator()(const Vector &w_o, const Vector &w_i) const {
	return (*bxdf)(w_o, flip_hemisphere(w_i));
}
Colorf BTDFAdapter::sample(const Vector &w_o, Vector &w_i, const std::array<float, 2> &samples, float &pdf_val) const {
	Colorf f = bxdf->sample(w_o, w_i, samples, pdf_val);
	w_i = flip_hemisphere(w_i);
	return f;
}
Colorf BTDFAdapter::rho_hd(const Vector &w_o, const std::array<float, 2> *samples, int n_samples) const {
	return bxdf->rho_hd(flip_hemisphere(w_o), samples, n_samples);
}
Colorf BTDFAdapter::rho_hh(const std::array<float, 2> *samples_a, const std::array<float, 2> *samples_b, int n_samples) const {
	return bxdf->rho_hh(samples_a, samples_b, n_samples);
}
float BTDFAdapter::pdf(const Vector &w_o, const Vector &w_i) const {
	return bxdf->pdf(w_o, flip_hemisphere(w_i));
}
Vector BTDFAdapter::flip_hemisphere(const Vector &v){
	return Vector{v.x, v.y, -v.z};
}

