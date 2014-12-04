#include "material/bsdf.h"
#include "material/blinn_distribution.h"
#include "material/anisotropic_distribution.h"
#include "material/torrance_sparrow.h"
#include "material/lambertian.h"
#include "material/fresnel.h"
#include "material/plastic_material.h"

PlasticMaterial::PlasticMaterial(const Texture *diffuse, const Texture *specular, float rough_x, float rough_y)
	: diffuse(diffuse), specular(specular), rough_x(rough_x), rough_y(rough_y)
{}
BSDF* PlasticMaterial::get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const {
	BSDF *bsdf = pool.alloc<BSDF>(dg);
	Colorf kd = diffuse->sample(dg).normalized();	
	if (!kd.is_black()){
		bsdf->add(pool.alloc<Lambertian>(kd));
	}
	Colorf ks = specular->sample(dg).normalized();
	if (!ks.is_black()){
		MicrofacetDistribution *distrib = nullptr;
		if (rough_y < 0){
			distrib = pool.alloc<BlinnDistribution>(1.f / rough_x);
		}
		else {
			distrib = pool.alloc<AnisotropicDistribution>(1.f / rough_x, 1.f / rough_y);
		}
		bsdf->add(pool.alloc<TorranceSparrow>(ks,
			pool.alloc<FresnelDielectric>(1.5f, 1.f), distrib));
	}
	return bsdf;
}

