#ifndef MERL_MATERIAL_H
#define MERL_MATERIAL_H

#include <string>
#include <vector>
#include "material.h"

/*
 * Material that uses measured data from "A Data-Driven Reflectance Model",
 * by Wojciech Matusik, Hanspeter Pfister, Matt Brand and Leonard McMillan,
 * in ACM Transactions on Graphics 22, 3(2003), 759-769.
 *
 * to model the surface reflection properties
 */
class MerlMaterial : public Material {
	int n_theta_h, n_theta_d, n_phi_d;
	//The measured brdf data in regular halfangle format
	std::vector<float> brdf;

public:
	/*
	 * Create the measured material, loading the BRDF data from some MERL
	 * binary BRDF file
	 */
	MerlMaterial(const std::string &file);
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry. Allocation of the BxDFs and BSDF will be done in the
	 * memory pool passed
	 */
	BSDF* get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const override;

private:
	/*
	 * Load the MERL BRDF data from a file, returns true if successful
	 * Implemented as described in PBR
	 */
	bool load_brdf(const std::string &file);
};

#endif

