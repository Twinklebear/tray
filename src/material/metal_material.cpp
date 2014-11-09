#include "material/bsdf.h"
#include "material/blinn_distribution.h"
#include "material/torrance_sparrow.h"
#include "material/lambertian.h"
#include "material/fresnel.h"
#include "material/metal_material.h"

MetalMaterial::MetalMaterial(const Texture *refr_index, const Texture *absoption_coef, float roughness)
	: refr_index(refr_index), absoption_coef(absoption_coef), roughness(roughness)
{}
BSDF* MetalMaterial::get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const {
	BSDF *bsdf = pool.alloc<BSDF>(dg);
	bsdf->add(pool.alloc<TorranceSparrow>(1.f,
		pool.alloc<FresnelConductor>(refr_index->sample(dg), absoption_coef->sample(dg)),
		pool.alloc<BlinnDistribution>(1.f / roughness)));
	return bsdf;
}

