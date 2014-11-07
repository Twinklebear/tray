#ifndef BXDF_H
#define BXDF_H

#include <array>
#include <vector>
#include <cmath>
#include "linalg/vector.h"
#include "linalg/util.h"
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
	virtual Colorf sample(const Vector &wo, Vector &wi, const std::array<float, 2> &samples, float &pdf_val) const;
	/*
	 * Compute the hemispherical-directional reflectance function using the samples passed
	 */
	virtual Colorf rho_hd(const Vector &wo, const std::array<float, 2> *samples, int n_samples) const;
	/*
	 * Compute the hemispherical-hemispherical reflectance function using the samples passed
	 * samples_a and samples_b should contain the same number of samples
	 */
	virtual Colorf rho_hh(const std::array<float, 2> *samples_a, const std::array<float, 2> *samples_b,
		int n_samples) const;
	/*
	 * Compute the probability density function for sampling the directions passed
	 */
	virtual float pdf(const Vector &wo, const Vector &wi) const;
	/*
	 * Quicker utilities for computing cos and sin the shading coordinate system
	 */
	static inline float cos_theta(const Vector &v){
		return v.z;
	}
	static inline float sin_theta2(const Vector &v){
		return std::max(0.f, 1.f - v.z * v.z);
	}
	static inline float sin_theta(const Vector &v){
		return std::sqrt(sin_theta2(v));
	}
	static inline float cos_phi(const Vector &v){
		float sintheta = sin_theta(v);
		if (sintheta == 0){
			return 1;
		}
		return clamp(v.x / sintheta, -1.f, 1.f);
	}
	static inline float sin_phi(const Vector &v){
		float sintheta = sin_theta(v);
		if (sintheta == 0){
			return 0;
		}
		return clamp(v.y / sintheta, -1.f, 1.f);
	}
	/*
	 * Check if two vectors are in the same hemisphere in shading space
	 */
	static inline bool same_hemisphere(const Vector &a, const Vector &b){
		return a.z * b.z > 0;
	}
};

#endif

