#ifndef MATERIAL_H
#define MATERIAL_H

#include "linalg/ray.h"
#include "cache.h"

class Material {
public:
	/*
	 * Apply the material shading properties to the hit object
	 * TODO: Need to return a color/spectrum struct
	 * TODO: Need to take a differential (or shading?) geometry
	 */
	virtual Point shade(const Ray &r) const = 0;
};

typedef Cache<Material> MaterialCache;

#endif

