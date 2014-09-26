#ifndef TEXTURE_H
#define TEXTURE_H

#include "cache.h"
#include "geometry/differential_geometry.h"
#include "render/color.h"

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
};

typedef Cache<Texture> TextureCache;

#endif

