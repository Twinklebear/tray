#ifndef OREN_NAYER_H
#define OREN_NAYER_H

#include "bxdf.h"

/*
 * BRDF describing the OrenNayer microfacet diffuse reflection model
 */
class OrenNayer : public BxDF {
	Colorf reflectance;
	float a, b;

public:
	/*
	 * Create an OrenNayer BRDF with some color and specify the sigma
	 * for the Gaussian microfacet distribution, sigma should be in degrees
	 */
	OrenNayer(const Colorf &reflectance, float sigma);
	/*
	 * Compute the value of the BxDF for some incident and outgoing directions
	 */
	Colorf operator()(const Vector &w_o, const Vector &w_i) const override;
};

#endif

