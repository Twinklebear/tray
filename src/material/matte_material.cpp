#include <memory>
#include "film/color.h"
#include "textures/texture.h"
#include "material/lambertian.h"
#include "material/oren_nayer.h"
#include "material/matte_material.h"

MatteMaterial::MatteMaterial(const Texture *diffuse, float roughness)
	: diffuse(diffuse), roughness(roughness)
{}
BSDF MatteMaterial::get_bsdf(const DifferentialGeometry &dg) const {
	BSDF bsdf{dg};
	Colorf kd = diffuse->sample(dg).normalized();
	if (roughness == 0){
		bsdf.add(std::make_unique<Lambertian>(kd));
	}
	else {
		bsdf.add(std::make_unique<OrenNayer>(kd, roughness));
	}
	return bsdf;
}

