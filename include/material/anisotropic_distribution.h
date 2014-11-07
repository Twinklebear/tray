#ifndef ANISOTROPIC_DISTRIBUTION_H
#define ANISOTROPIC_DISTRIBUTION_H

#include "microfacet_distribution.h"

/*
 * Implementation of the anisotropic microfacet distribution described by
 * Ashikhmin and Shirley
 */
class AnisotropicDistribution : public MicrofacetDistribution {
	//The exponents for microfacets exactly along the x/y axes
	float exp_x, exp_y;

public:
	/*
	 * Create the anisotropic distribution specifying the exponents for microfacets
	 * oriented exactly along the x and y axes
	 */
	AnisotropicDistribution(float exp_x, float exp_y);
	/*
	 * Compute the probability density for microfacets to be
	 * oriented with normal = w_h in this distribution
	 */
	float operator()(const Vector &w_h) const override;
	/*
	 * Sample the distribution for some outgoing direction, returning the incident
	 * direction and the PDF for this pair of vectors
	 */
	void sample(const Vector &w_o, Vector &w_i, const std::array<float, 2> &u, float &pdf_val) const override;
	/*
	 * Sample the PDF of the distribution for some pair of directions
	 */
	float pdf(const Vector &w_o, const Vector &w_i) const override;
};

#endif

