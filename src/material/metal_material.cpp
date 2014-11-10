#include "material/bsdf.h"
#include "material/anisotropic_distribution.h"
#include "material/torrance_sparrow.h"
#include "material/lambertian.h"
#include "material/fresnel.h"
#include "material/metal_material.h"

MetalMaterial::MetalMaterial(const Texture *refr_index, const Texture *absoption_coef, float rough_x, float rough_y)
	: refr_index(refr_index), absoption_coef(absoption_coef), rough_x(rough_x), rough_y(rough_y)
{}
BSDF* MetalMaterial::get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const {
	BSDF *bsdf = pool.alloc<BSDF>(dg);
	bsdf->add(pool.alloc<TorranceSparrow>(1.f,
		pool.alloc<FresnelConductor>(refr_index->sample(dg), absoption_coef->sample(dg)),
		pool.alloc<AnisotropicDistribution>(1.f / rough_x, 1.f / rough_y)));
	return bsdf;
}

