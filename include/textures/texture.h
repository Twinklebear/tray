#ifndef TEXTURE_H
#define TEXTURE_H

#include "cache.h"
#include "geometry/differential_geometry.h"
#include "film/color.h"
#include "texture_mapping.h"

/*
 * Interface for texture, provides method to sample the color of the
 * for some differential geometry being rendered
 */
class Texture {
public:
	/*
	 * Sample the texture color for the piece of geometry being textured
	 */
	virtual Colorf sample(const DifferentialGeometry &dg) const = 0;
	/*
	 * Sometimes we want to re-use a texture but through a different mapping
	 * This method samples the texture directly using the texture sample passed in
	 */
	virtual Colorf sample(const TextureSample &sample) const = 0;
};

typedef Cache<Texture> TextureCache;

#endif

