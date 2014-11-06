#include <cmath>
#include "material/bxdf.h"
#include "material/microfacet_distribution.h"


float MicrofacetDistribution::geom_atten(const Vector &w_o, const Vector &w_i, const Vector &w_h) const {
	float n_dot_h = std::abs(BxDF::cos_theta(w_h));
	float n_dot_o = std::abs(BxDF::cos_theta(w_o));
	float n_dot_i = std::abs(BxDF::cos_theta(w_i));
	float o_dot_h = std::abs(w_o.dot(w_h));
	return std::min(1.f, std::min(2 * n_dot_h * n_dot_o / o_dot_h, 2 * n_dot_h * n_dot_i / o_dot_h));
}

