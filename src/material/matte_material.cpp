#include <memory>
#include "film/color.h"
#include "textures/texture.h"
#include "material/lambertian.h"
#include "material/oren_nayer.h"
#include "material/matte_material.h"
#include "material/specular_reflection.h"
#include "material/fresnel.h"

MatteMaterial::MatteMaterial(const Texture *diffuse, float roughness)
	: diffuse(diffuse), roughness(roughness)
{}
BSDF* MatteMaterial::get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const {
	BSDF *bsdf = pool.alloc<BSDF>(dg);
	Colorf kd = diffuse->sample(dg).normalized();
	if (!kd.is_black()){
		if (roughness == 0){
			bsdf->add(pool.alloc<Lambertian>(kd));
		}
		else {
			bsdf->add(pool.alloc<OrenNayer>(kd, roughness));
		}
	}
	return bsdf;
}

