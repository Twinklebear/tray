#ifndef SCALED_BXDF_H
#define SCALED_BXDF_H

#include "bxdf.h"

/*
 * A BxDF that scales the output of the given BxDF by some color
 */
class ScaledBxDF : public BxDF {
	BxDF *bxdf;
	Colorf scale;

public:
	/*
	 * Create the scaled BxDF to apply the desired scaling to the output
	 * of the BxDF passed
	 */
	ScaledBxDF(BxDF *bxdf, const Colorf &scale);
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
};

#endif

