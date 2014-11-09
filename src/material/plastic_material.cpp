#include "material/bsdf.h"
#include "material/blinn_distribution.h"
#include "material/torrance_sparrow.h"
#include "material/lambertian.h"
#include "material/fresnel.h"
#include "material/plastic_material.h"

PlasticMaterial::PlasticMaterial(const Texture *diffuse, const Texture *specular, float roughness)
	: diffuse(diffuse), specular(specular), roughness(roughness)
{}
BSDF* PlasticMaterial::get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const {
	BSDF *bsdf = pool.alloc<BSDF>(dg);
	Colorf kd = diffuse->sample(dg).normalized();	
	if (!kd.is_black()){
		bsdf->add(pool.alloc<Lambertian>(kd));
	}
	Colorf ks = specular->sample(dg).normalized();
	if (!ks.is_black()){
		bsdf->add(pool.alloc<TorranceSparrow>(ks,
			pool.alloc<FresnelDielectric>(1.5f, 1.f),
			pool.alloc<BlinnDistribution>(1.f / roughness)));
	}
	return bsdf;
}

