#ifndef BLINN_DISTRIBUTION_H
#define BLINN_DISTRIBUTION_H

#include "microfacet_distribution.h"

/*
 * Implementation of the microfacet distribution described by Blinn
 */
class BlinnDistribution : public MicrofacetDistribution {
	float exponent;

public:
	/*
	 * Create the microfacet distribution with some exponent, the higher
	 * the exponent the more shiny the surface appears
	 */
	BlinnDistribution(float exponent);
	/*
	 * Compute the probability density for microfacets to be
	 * oriented with normal = w_h in this distribution
	 */
	float operator()(const Vector &w_h) const override;
};

#endif

