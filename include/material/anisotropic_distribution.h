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
};

#endif

