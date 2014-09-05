#ifndef FLAT_MATERIAL_H
#define FLAT_MATERIAL_H

#include "material.h"

class FlatMaterial : public Material {
	Colorf color;

public:
	/*
	 * Create a basic flat material that will simply return
	 * the color set
	 */
	FlatMaterial(const Colorf &color);
	/*
	 * "Shade" the object, just returns the color set
	 */
	Colorf shade(const Ray &r, const HitInfo &hitinfo, const LightCache &lights) const override;
};

#endif

