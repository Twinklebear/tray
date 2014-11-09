#ifndef MICROFACET_DISTRIBUTION_H
#define MICROFACET_DISTRIBUTION_H

#include "linalg/vector.h"

/*
 * Describes the distribution of microfacets on a surface for microfacet
 * based reflectance models
 */
class MicrofacetDistribution {
public:
	/*
	 * Compute the probability density for microfacets to be
	 * oriented with normal = w_h in this distribution
	 */
	virtual float operator()(const Vector &w_h) const = 0;
	/*
	 * Sample the distribution for some outgoing direction, returning the incident
	 * direction and the PDF for this pair of vectors
	 */
	virtual void sample(const Vector &w_o, Vector &w_i, const std::array<float, 2> &u, float &pdf_val) const = 0;
	/*
	 * Sample the PDF of the distribution for some pair of directions
	 */
	virtual float pdf(const Vector &w_o, const Vector &w_i) const = 0;
	/*
	 * Compute the geometric attenuation term for the distribution
	 * for the outgoing and incident directions for the microfacets
	 * with normal = w_h
	 */
	static float geom_atten(const Vector &w_o, const Vector &w_i, const Vector &w_h);
};

#endif

