#ifndef MATERIAL_H
#define MATERIAL_H

#include "linalg/ray.h"
#include "geometry/hitinfo.h"
#include "render/color.h"
#include "lights/light.h"
#include "cache.h"

class Material {
public:
	/*
	 * Apply the material shading properties to the hit object
	 * TODO: Need to take a differential (or shading?) geometry
	 * the hitinfo fills this need, but do we need anything more complicated?
	 */
	virtual Colorf shade(const Ray &r, const HitInfo &hitinfo, const LightCache &lights) const = 0;
};

typedef Cache<Material> MaterialCache;

#endif

