#ifndef REMAPPED_TEXTURE_H
#define REMAPPED_TEXTURE_H

#include "texture.h"
#include "texture_mapping.h"

/*
 * A bit of a hack, allows for re-using an existing texture but
 * with a different texture mapping
 */
class RemappedTexture : public Texture {
	const Texture &texture;
	std::unique_ptr<TextureMapping> mapping;

public:
	/*
	 * Create the remapped texture to apply sample an existing texture
	 * with a new mapping scheme
	 */
	RemappedTexture(const Texture &texture, std::unique_ptr<TextureMapping> mapping);
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

