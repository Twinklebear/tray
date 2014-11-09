#ifndef BTDF_ADAPTER_H
#define BTDF_ADAPTER_H

#include "bxdf.h"

/*
 * Take an existing BRDF model and use it as a transmissive model,
 * or the other way around. This adapter flips the reflection and transmission
 * type flags and flips incident vectors to the other hemisphere
 */
class BTDFAdapter : public BxDF {
	BxDF *bxdf;

public:
	/*
	 * Create the adapter to adapt the existing BxDF model from reflection to transmission
	 * or vice-versa
	 */
	BTDFAdapter(BxDF *b);
	/*
	 * Compute the value of the BxDF for some incident and outgoing directions
	 */
	Colorf operator()(const Vector &w_o, const Vector &w_i) const override;
	/*
	 * Sample the BxDFs value for some outgoing direction using the random values
	 * passed and returning the incident light direction
	 */
	Colorf sample(const Vector &w_o, Vector &w_i, const std::array<float, 2> &samples, float &pdf_val) const override;
	/*
	 * Compute the hemispherical-directional reflectance function using the samples passed
	 */
	Colorf rho_hd(const Vector &w_o, const std::array<float, 2> *samples, int n_samples) const override;
	/*
	 * Compute the hemispherical-hemispherical reflectance function using the samples passed
	 * samples_a and samples_b should contain the same number of samples
	 */
	Colorf rho_hh(const std::array<float, 2> *samples_a, const std::array<float, 2> *samples_b,
		int n_samples) const override;
	/*
	 * Compute the probability density function for sampling the directions passed
	 */
	float pdf(const Vector &w_o, const Vector &w_i) const override;
	/*
	 * Flip the vector to the other hemisphere in shading space
	 */
	static Vector flip_hemisphere(const Vector &v);
};

#endif

