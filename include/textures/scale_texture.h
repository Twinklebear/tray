#ifndef SCALE_TEXTURE_H
#define SCALE_TEXTURE_H

#include "texture.h"

/*
 * Texture that multiplies two textures together to form the final color
 */
class ScaleTexture : public Texture {
	const Texture &a, &b;

public:
	/*
	 * Create the scale texture and set the two textures to be multiplied together
	 * These textures aren't owned by the scale texture and should live in the cache
	 */
	ScaleTexture(const Texture &a, const Texture &b);
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

