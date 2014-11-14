#ifndef METAL_MATERIAL_H
#define METAL_MATERIAL_H

#include "textures/texture.h"
#include "material.h"

/*
 * A material that models a metal surface, described by its
 * refractive index and absorption coefficient
 */
class MetalMaterial : public Material {
	const Texture *refr_index, *absoption_coef;
	const float rough_x, rough_y;

public:
	/*
	 * Create the metal, specifying the textures to be used for its attributes
	 * To create an anisotropic material specify both rough_x and rough_y, for a
	 * non-aniostropic material only specify rough_x
	 */
	MetalMaterial(const Texture *refr_index, const Texture *absoption_coef, float rough_x, float rough_y = -1);
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry
	 */
	BSDF* get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const override;
};

#endif

