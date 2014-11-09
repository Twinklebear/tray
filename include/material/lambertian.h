#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "bxdf.h"

/*
 * BRDF describing the Lambertian diffuse reflectance model
 */
class Lambertian : public BxDF {
	Colorf reflectance;

public:
	/*
	 * Create a Lambertian BRDF with some color
	 */
	Lambertian(const Colorf &reflectance);
	/*
	 * Compute the value of the BxDF for some incident and outgoing directions
	 */
	Colorf operator()(const Vector &w_o, const Vector &w_i) const override;
	/*
	 * Compute the hemispherical-directional reflectance function using the samples passed
	 */
	Colorf rho_hd(const Vector &w_o, const std::array<float, 2> *samples, int n_samples) const override;
	/*
	 * Compute the hemispherical-hemispherical reflectance function using the samples passed
	 */
	Colorf rho_hh(const std::array<float, 2> *samples_a, const std::array<float, 2> *samples_b,
		int n_samples) const override;
};

#endif

