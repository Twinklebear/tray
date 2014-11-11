#ifndef MERL_BRDF_H
#define MERL_BRDF_H

#include <vector>
#include "linalg/util.h"
#include "bxdf.h"

/*
 * BRDF that uses measured data from "A Data-Driven Reflectance Model",
 * by Wojciech Matusik, Hanspeter Pfister, Matt Brand and Leonard McMillan,
 * in ACM Transactions on Graphics 22, 3(2003), 759-769.
 *
 * to model the surface reflection properties. Implemnentation based on
 * the method introduced in PBR
 */
class MerlBRDF : public BxDF {
	//The measured brdf data in regular halfangle format
	const std::vector<float> &brdf;
	const int n_theta_h, n_theta_d, n_phi_d;

public:
	/*
	 * Create the Merl BRDF using some loaded data for modeling the BRDF
	 */
	MerlBRDF(const std::vector<float> &brdf, int n_theta_h, int n_theta_d, int n_phi_d);
	/*
	 * Compute the value of the BxDF for some incident and outgoing directions
	 */
	Colorf operator()(const Vector &w_o, const Vector &w_i) const override;

private:
	/*
	 * Remap values from the angular value to the index in the MERL BRDF data table
	 */
	static constexpr inline int map_index(float val, float max, int n_vals){
		return clamp(static_cast<int>(val / max * n_vals), 0, n_vals - 1);
	}
};

#endif

