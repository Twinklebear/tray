#ifndef TORRANCE_SPARROW_H
#define TORRANCE_SPARROW_H

#include <array>
#include "bxdf.h"
#include "microfacet_distribution.h"
#include "fresnel.h"

/*
 * Implementation of the Torrance-Sparrow microfacet BRDF model
 */
class TorranceSparrow : public BxDF {
	Colorf reflectance;
	Fresnel *fresnel;
	MicrofacetDistribution *distribution;

public:
	/*
	 * Create the microface BRDF with some color, fresnel term
	 * and desired microfacet distribution
	 */
	TorranceSparrow(const Colorf &reflectance, Fresnel *fresnel, MicrofacetDistribution *distribution);
	/*
	 * Compute the value of the BxDF for some incident and outgoing directions
	 */
	Colorf operator()(const Vector &wo, const Vector &wi) const override;
	/*
	 * Sample the BxDFs value for some outgoing direction using the random values
	 * passed and returning the incident light direction
	 */
	Colorf sample(const Vector &w_o, Vector &w_i, const std::array<float, 2> &samples, float &pdf_val) const override;
	/*
	 * Compute the probability density function for sampling the directions passed
	 */
	float pdf(const Vector &w_o, const Vector &w_i) const override;
};

#endif

