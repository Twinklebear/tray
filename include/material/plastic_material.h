#ifndef PLASTIC_MATERIAL_H
#define PLASTIC_MATERIAL_H

#include "textures/texture.h"
#include "material.h"

/*
 * A material that models plastic, parameterized by diffuse and specular
 * colors along with a roughness value
 */
class PlasticMaterial : public Material {
	const Texture *diffuse, *specular;
	const float rough_x, rough_y;

public:
	/*
	 * Create the plastic material specifying the textures to be used for
	 * the diffuse and specular colors
	 */
	PlasticMaterial(const Texture *diffuse, const Texture *specular, float rough_x, float rough_y);
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry
	 */
	BSDF* get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const override;
};

#endif

