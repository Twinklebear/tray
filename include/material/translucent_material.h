#ifndef TRANSLUCENT_MATERIAL_H
#define TRANSLUCENT_MATERIAL_H

#include "textures/texture.h"
#include "pbr_material.h"

/*
 * A material modeling a translucent dielectric, eg. transparent
 * plastics or similar
 */
class TranslucentMaterial : public PBRMaterial {
	const Texture *diffuse, *specular, *reflect, *transmit;
	const float roughness, refr_index;

public:
	/*
	 * Create the translucent material specifying the textures to be used
	 * for the colors of each property and the roughness
	 */
	TranslucentMaterial(const Texture *diffuse, const Texture *specular, const Texture *reflect,
		const Texture *transmit, float roughness, float refr_index);
	/*
	 * Get the BSDF to compute the shading for the material at this
	 * piece of geometry
	 */
	BSDF* get_bsdf(const DifferentialGeometry &dg, MemoryPool &pool) const override;
};

#endif

