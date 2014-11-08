#ifndef BSDF_H
#define BSDF_H

#include <random>
#include <memory>
#include <vector>
#include "memory_pool.h"
#include "linalg/vector.h"
#include "samplers/sampler.h"
#include "geometry/differential_geometry.h"
#include "film/color.h"
#include "bxdf.h"

/*
 * Implementation of BSDF to manage surface's BRDT and BTDFs motivated by PBR
 */
class BSDF {
	//Vectors forming the shading coordinate system:
	//shading normal, secondary and primary tangents along with the geometry normal
	//the coordinate system has the shading normal as z, primary tangent as x and secondary as y
	Normal normal, geom_normal;
	Vector bitangent, tangent;
	std::array<BxDF*, 8> bxdfs;
	int n_bxdfs;

public:
	const DifferentialGeometry dg;
	const float eta;

	/*
	 * Construct the BSDF for the differential geometry being shaded
	 */
	BSDF(const DifferentialGeometry &dg, float eta = 1);
	/*
	 * Add a BxDF to the set of BxDFs used by this BSDF
	 */
	void add(BxDF *b);
	/*
	 * Return the number of BxDFs in this BSDF or those specifically matching a flag
	 */
	int num_bxdfs() const;
	int num_bxdfs(BxDFTYPE flags) const;
	/*
	 * Transform to and from the shading space used by the BxDFs
	 */
	Vector to_shading(const Vector &v) const;
	Vector from_shading(const Vector &v) const;
	/*
	 * Evaluate the BxDFs at the surface for the incoming and outgoing light directions
	 * light directions should be in world space
	 * Can optionally specify BxDFTYPE flag filters, default is all
	 */
	Colorf operator()(const Vector &wo_world, const Vector &wi_world, BxDFTYPE flags = BxDFTYPE::ALL) const;
	/*
	 * Sample a BxDF in the BSDF, comp is used to randomly select the BxDF and u is used to sample
	 * the BxDF itself
	 * Returns information about the sampled color, incident direction, pdf and optionally the
	 * sampled type of the sampled BxDF
	 */
	Colorf sample(const Vector &wo_world, Vector &wi_world, const std::array<float, 2> &u, float comp,
		float &pdf_val, BxDFTYPE flags = BxDFTYPE::ALL, BxDFTYPE *sampled_type = nullptr) const;
	/*
	 * Sample the hemispherical-directional reflectance function of the BxDFs
	 * sqrt_samples is the square root of the number of samples to take
	 */
	Colorf rho_hd(const Vector &wo, Sampler &sampler, MemoryPool &pool,
		BxDFTYPE flags = BxDFTYPE::ALL, int sqrt_samples = 6) const;
	/*
	 * Sample the hemispherical-hemispherical reflectance function of the BxDFs
	 */
	Colorf rho_hh(Sampler &sampler, MemoryPool &pool, BxDFTYPE flags = BxDFTYPE::ALL, int sqrt_samples = 6) const;
	/*
	 * Compute the probability density function for BxDFs for the directions passed
	 */
	float pdf(const Vector &wo_world, const Vector &wi_world, BxDFTYPE flags = BxDFTYPE::ALL) const;

private:
	/*
	 * Find the ith BxDF matching the desired flags, returns nullptr if no matches
	 * or i >= num_bxdfs(flags)
	 */
	const BxDF* matching_at(int i, BxDFTYPE flags) const;
};

#endif

