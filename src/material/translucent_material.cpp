#include "material/bsdf.h"
#include "material/blinn_distribution.h"
#include "material/torrance_sparrow.h"
#include "material/lambertian.h"
#include "material/fresnel.h"
#include "material/btdf_adapter.h"
#include "material/translucent_material.h"

TranslucentMaterial::TranslucentMaterial(const Texture *diffuse, const Texture *specular, const Texture *reflect,
	const Texture *transmit, float roughness, float refr_index)
	: diffuse(diffuse), specular(specular), reflect(reflect), transmit(transmit),
	roughness(roughness), refr_index(refr_index)
{}
BSDF* TranslucentMaterial::get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const {
	BSDF *bsdf = pool.alloc<BSDF>(dg, refr_index);
	Colorf kr = reflect->sample(dg).normalized();
	Colorf kt = transmit->sample(dg).normalized();
	if (kr.is_black() && kt.is_black()){
		return bsdf;
	}
	Colorf kd = diffuse->sample(dg).normalized();	
	if (!kd.is_black()){
		if (!kr.is_black()){
			bsdf->add(pool.alloc<Lambertian>(kr * kd));
		}
		if (!kt.is_black()){
			bsdf->add(pool.alloc<BTDFAdapter>(pool.alloc<Lambertian>(kt * kd)));
		}
	}
	Colorf ks = specular->sample(dg).normalized();
	if (!ks.is_black()){
		if (!kr.is_black()){
			bsdf->add(pool.alloc<TorranceSparrow>(kr * ks,
				pool.alloc<FresnelDielectric>(refr_index, 1.f),
				pool.alloc<BlinnDistribution>(1.f / roughness)));
		}
		if (!kt.is_black()){
			bsdf->add(pool.alloc<BTDFAdapter>(pool.alloc<TorranceSparrow>(kt * ks,
				pool.alloc<FresnelFlip>(pool.alloc<FresnelDielectric>(refr_index, 1.f)),
				pool.alloc<BlinnDistribution>(1.f / roughness))));
		}
	}
	return bsdf;
}

