#ifndef METAL_MATERIAL_H
#define METAL_MATERIAL_H

#include "textures/texture.h"
#include "pbr_material.h"

/*
 * A material that models a metal surface, described by its
 * refractive index and absorption coefficient
 */
class MetalMaterial : public PBRMaterial {
	const Texture *refr_index, *absoption_coef;
	const float roughness;

public:
	/*
	 * Create the metal, specifying the textures to be used for its attributes
	 */
	MetalMaterial(const Texture *refr_index, const Texture *absoption_coef, float roughness);
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry
	 */
	BSDF* get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const override;
};

#endif

