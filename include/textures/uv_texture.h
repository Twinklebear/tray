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
	/*
	 * Sometimes we want to re-use a texture but through a different mapping
	 * This method samples the texture directly using the texture sample passed in
	 */
	Colorf sample(const TextureSample &sample) const override;
};

#endif

