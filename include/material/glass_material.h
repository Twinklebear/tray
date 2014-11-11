#ifndef GLASS_MATERIAL_H
#define GLASS_MATERIAL_H

#include "textures/texture.h"
#include "material.h"

/*
 * A material describing perfectly smooth glass with specular reflection
 * and transmission
 */
class GlassMaterial : public Material {
	const Texture *reflect, *transmit;
	const float refr_index;

public:
	/*
	 * Create the glass material, specifying the colors and refractive index
	 */
	GlassMaterial(const Texture *reflect, const Texture *transmit, float refr_index);
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry
	 */
	BSDF* get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const override;
};

#endif

