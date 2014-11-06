#ifndef TORRANCE_SPARROW_H
#define TORRANCE_SPARROW_H

#include <memory>
#include "bxdf.h"
#include "microfacet_distribution.h"
#include "fresnel.h"

/*
 * Implementation of the Torrance-Sparrow microfacet BRDF model
 */
class TorranceSparrow : public BxDF {
	Colorf reflectance;
	std::unique_ptr<Fresnel> fresnel;
	std::unique_ptr<MicrofacetDistribution> distribution;

public:
	/*
	 * Create the microface BRDF with some color, fresnel term
	 * and desired microfacet distribution
	 */
	TorranceSparrow(const Colorf &reflectance, std::unique_ptr<Fresnel> fresnel,
		std::unique_ptr<MicrofacetDistribution> distribution);
	/*
	 * Compute the value of the BxDF for some incident and outgoing directions
	 */
	Colorf operator()(const Vector &wo, const Vector &wi) const override;
};

#endif

