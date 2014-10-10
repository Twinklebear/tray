#ifndef CONSTANT_TEXTURE_H
#define CONSTANT_TEXTURE_H

#include "texture.h"

/*
 * A texture with the same value everywhere
 */
class ConstantTexture : public Texture {
	Colorf color;

public:
	/*
	 * Create the constant valued texture and assign the value to show
	 */
	ConstantTexture(const Colorf &color);
	/*
	 * Sample the texture color for the piece of geometry being textured
	 */
	Colorf sample(const DifferentialGeometry &dg) const override;
	/*
	 * Sometimes we want to re-use a texture but through a different mapping
	 * This method samples the texture directly using the texture sample passed in
	 */
	Colorf sample(const TextureSample &sample) const override;
};

#endif

