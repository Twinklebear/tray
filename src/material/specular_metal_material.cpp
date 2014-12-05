#include "material/bsdf.h"
#include "material/fresnel.h"
#include "material/specular_reflection.h"
#include "material/specular_metal_material.h"

SpecularMetalMaterial::SpecularMetalMaterial(const Texture *refr_index, const Texture *absoption_coef)
	: refr_index(refr_index), absoption_coef(absoption_coef)
{}
BSDF* SpecularMetalMaterial::get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const {
	BSDF *bsdf = pool.alloc<BSDF>(dg);
	bsdf->add(pool.alloc<SpecularReflection>(1.f,
		pool.alloc<FresnelConductor>(refr_index->sample(dg), absoption_coef->sample(dg))));
	return bsdf;
}

