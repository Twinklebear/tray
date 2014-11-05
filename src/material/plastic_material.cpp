#include <memory>
#include "film/color.h"
#include "material/plastic_material.h"

PlasticMaterial::PlasticMaterial(const Texture *diffuse, const Texture *specular, float roughness)
	: diffuse(diffuse), specular(specular), roughness(roughness)
{}
BSDF PlasticMaterial::get_bsdf(const DifferentialGeometry &dg) const {
	//TODO: Need Blinn Microfacet BRDF for this material
}

