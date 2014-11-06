#include <memory>
#include "film/color.h"
#include "material/bsdf.h"
#include "material/blinn_distribution.h"
#include "material/torrance_sparrow.h"
#include "material/lambertian.h"
#include "material/fresnel.h"
#include "material/plastic_material.h"

PlasticMaterial::PlasticMaterial(const Texture *diffuse, const Texture *specular, float roughness)
	: diffuse(diffuse), specular(specular), roughness(roughness)
{}
BSDF PlasticMaterial::get_bsdf(const DifferentialGeometry &dg) const {
	BSDF bsdf{dg};
	Colorf kd = diffuse->sample(dg).normalized();	
	Colorf ks = specular->sample(dg).normalized();
	bsdf.add(std::make_unique<Lambertian>(kd));
	bsdf.add(std::make_unique<TorranceSparrow>(ks,
		std::make_unique<FresnelDielectric>(1.5, 1),
		std::make_unique<BlinnDistribution>(1.f / roughness)));
	return bsdf;
}

