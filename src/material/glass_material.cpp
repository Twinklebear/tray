#include "film/color.h"
#include "textures/texture.h"
#include "material/specular_reflection.h"
#include "material/specular_transmission.h"
#include "material/fresnel.h"
#include "material/glass_material.h"

GlassMaterial::GlassMaterial(const Texture *reflect, const Texture *transmit, float refr_index)
	: reflect(reflect), transmit(transmit), refr_index(refr_index)
{}
BSDF* GlassMaterial::get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const {
	BSDF *bsdf = pool.alloc<BSDF>(dg, refr_index);
	Colorf refl = reflect->sample(dg);
	if (!refl.is_black()){
		bsdf->add(pool.alloc<SpecularReflection>(refl,
			pool.alloc<FresnelDielectric>(1.f, refr_index)));
	}
	Colorf trans = transmit->sample(dg);
	if (!trans.is_black()){
		bsdf->add(pool.alloc<SpecularTransmission>(trans,
			pool.alloc<FresnelDielectric>(1.f, refr_index)));
	}
	return bsdf;
}

