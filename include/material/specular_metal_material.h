#ifndef SPECULAR_METAL_MATERIAL
#define SPECULAR_METAL_MATERIAL

#include "textures/texture.h"
#include "material.h"

/*
 * A material that models a perfectly specular metal surface, described by its
 * refractive index and absorption coefficient
 */
class SpecularMetalMaterial : public Material {
	const Texture *refr_index, *absoption_coef;

public:
	/*
	 * Create the metal, specifying the textures to be used for its attributes
	 * To create an anisotropic material specify both rough_x and rough_y, for a
	 * non-aniostropic material only specify rough_x
	 */
	SpecularMetalMaterial(const Texture *refr_index, const Texture *absoption_coef);
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry
	 */
	BSDF* get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const override;
};

#endif

