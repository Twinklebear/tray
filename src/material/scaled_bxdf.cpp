#include "material/scaled_bxdf.h"

ScaledBxDF::ScaledBxDF(BxDF *bxdf, const Colorf &scale) : BxDF(bxdf->type), bxdf(bxdf), scale(scale){}
Colorf ScaledBxDF::operator()(const Vector &w_o, const Vector &w_i) const {
	return scale * (*bxdf)(w_o, w_i);
}
Colorf ScaledBxDF::sample(const Vector &w_o, Vector &w_i, const std::array<float, 2> &samples, float &pdf_val) const {
	return scale * bxdf->sample(w_o, w_i, samples, pdf_val);
}
Colorf ScaledBxDF::rho_hd(const Vector &w_o, const std::array<float, 2> *samples, int n_samples) const {
	return scale * bxdf->rho_hd(w_o, samples, n_samples);
}
Colorf ScaledBxDF::rho_hh(const std::array<float, 2> *samples_a, const std::array<float, 2> *samples_b, int n_samples) const {
	return scale * bxdf->rho_hh(samples_a, samples_b, n_samples);
}
float ScaledBxDF::pdf(const Vector &w_o, const Vector &w_i) const {
	return bxdf->pdf(w_o, w_i);
}

