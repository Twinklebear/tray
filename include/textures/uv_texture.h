#ifndef UV_TEXTURE_H
#define UV_TEXTURE_H

#include <memory>
#include "texture.h"
#include "texture_mapping.h"

/*
 * Procedural texture that displays the st texture coordinates
 * as RG color values
 */
class UVTexture : public Texture {
	std::unique_ptr<TextureMapping> mapping;

public:
	UVTexture(std::unique_ptr<TextureMapping> mapping);
	/*
	 * Sample the texture color for the piece of geometry being textured
	 */
	Colorf sample(const DifferentialGeometry &dg) const override;
};

#endif

