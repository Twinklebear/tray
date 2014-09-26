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
};

#endif

