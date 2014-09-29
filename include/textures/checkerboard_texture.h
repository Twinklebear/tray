#ifndef CHECKERBOARD_TEXTURE_H
#define CHECKERBOARD_TEXTURE_H

#include <memory>
#include "texture.h"
#include "texture_mapping.h"

/*
 * Displays a checkerboard of the two color values
 */
class CheckerboardTexture : public Texture {
	std::unique_ptr<TextureMapping> mapping;
	Colorf a, b;

public:
	CheckerboardTexture(const Colorf &a, const Colorf &b,
		std::unique_ptr<TextureMapping> mapping);
	/*
	 * Sample the texture color for the piece of geometry being textured
	 */
	Colorf sample(const DifferentialGeometry &dg) const override;
};

#endif

