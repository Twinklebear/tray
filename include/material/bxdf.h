#ifndef BXDF_H
#define BXDF_H

#include <array>
#include <vector>
#include "linalg/vector.h"
#include "film/color.h"

/*
 * Implementation of BxDF motivated by PBR
 */
/*
 * Describes which type of light interaction the BxDF computes
 */
enum BxDFTYPE {
	REFLECTION = 1,
	TRANSMISSION = 1 << 1,
	DIFFUSE = 1 << 2,
	GLOSSY = 1 << 3,
	SPECULAR = 1 << 4,
	ALL_TYPES = DIFFUSE | GLOSSY | SPECULAR,
	ALL_REFLECTION = REFLECTION | ALL_TYPES,
	ALL_TRANSMISSION = TRANSMISSION | ALL_TYPES,
	ALL = ALL_REFLECTION | ALL_TRANSMISSION
};

/*
 * Generic BxDF class for interacting with BRDFs and BTDFs in a general
 */
class BxDF {
public:
	const BxDFTYPE type;

	BxDF(BxDFTYPE t);
	/*
	 * Check if this BxDF matches the type flags
	 */
	bool matches(BxDFTYPE flags) const;
	/*
	 * Compute the value of the BxDF for some incident and outgoing directions
	 */
	virtual Colorf operator()(const Vector &wo, const Vector &wi) const = 0;
	/*
	 * Sample the BxDFs value for some outgoing direction using the random values
	 * passed and returning the incident light direction
	 */
	virtual Colorf sample(const Vector &wo, Vector &wi, const std::array<float, 2> &samples, float &pdf) const;
	/*
	 * Compute the hemispherical-directional reflectance function using the samples passed
	 */
	virtual Colorf rho_hd(const Vector &wo, const std::vector<float> &samples) const;
	/*
	 * Compute the hemispherical-hemispherical reflectance function using the samples passed
	 */
	virtual Colorf rho_hh(const std::vector<float> &samples_a, const std::vector<float> &samples_b) const;
	/*
	 * Compute the probability density function for sampling the directions passed
	 */
	virtual float pdf(const Vector &wo, const Vector &wi) const;
};

#endif

