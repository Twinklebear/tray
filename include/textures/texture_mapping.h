#ifndef TEXTURE_MAPPING_H
#define TEXTURE_MAPPING_H

#include "geometry/differential_geometry.h"

/*
 * Stores information about the s,t coords and derivatives
 * to evaluat a texture's value
 */
struct TextureSample {
	float s, t, ds_dx, ds_dy, dt_dx, dt_dy;

	TextureSample(float s = 0, float t = 0, float ds_dx = 0,
		float ds_dy = 0, float dt_dx = 0, float dt_dy = 0);
};

/*
 * Interface for mapping u,v coords to s,t texture coords
 */
class TextureMapping {
public:
	/*
	 * Compute the texture sample position and derivatives for the
	 * differential geometry being rendered
	 */
	virtual TextureSample map(const DifferentialGeometry &dg) const = 0;
};

#endif

