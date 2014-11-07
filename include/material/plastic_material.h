#ifndef PLASTIC_MATERIAL_H
#define PLASTIC_MATERIAL_H

#include "textures/texture.h"
#include "pbr_material.h"

/*
 * A material that models plastic, parameterized by diffuse and specular
 * colors along with a roughness value
 */
class PlasticMaterial : public PBRMaterial {
	const Texture *diffuse, *specular;
	const float roughness;

public:
	/*
	 * Create the plastic material specifying the textures to be used for
	 * the diffuse and specular colors
	 */
	PlasticMaterial(const Texture *diffuse, const Texture *specular, float roughness);
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry
	 */
	BSDF* get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const override;
};

#endif

