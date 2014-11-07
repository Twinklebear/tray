#ifndef SPECULAR_TRANSMISSION_H
#define SPECULAR_TRANSMISSION_H

#include "fresnel.h"
#include "bxdf.h"

/*
 * BTDF describing perfectly specular transmission eg. a delta distribution
 */
class SpecularTransmission : public BxDF {
	Colorf transmission;
	FresnelDielectric *fresnel;

public:
	/*
	 * Create a specularly transmissive BTDF with some transmissive color
	 * and desired Fresnel component
	 */
	SpecularTransmission(const Colorf &transmission, FresnelDielectric *fresnel);
	/*
	 * Compute the value of the BxDF for some incident and outgoing directions
	 * Note that because this is a delta distribution this will always return 0,
	 * sample must be used for delta distributions instead
	 */
	Colorf operator()(const Vector &wo, const Vector &wi) const override;
	/*
	 * Sample the BTDFs value for some outgoing direction using the random values
	 * passed and returning the incident light direction
	 */
	Colorf sample(const Vector &wo, Vector &wi, const std::array<float, 2> &samples, float &pdf_val) const override;
	/*
	 * Compute the probability density function for sampling the directions passed
	 * Note again that this returns 0 because this is a delta distribution
	 */
	float pdf(const Vector &wo, const Vector &wi) const override;
};

#endif

