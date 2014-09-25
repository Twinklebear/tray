#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>
#include "linalg/ray.h"
#include "geometry/differential_geometry.h"
#include "render/color.h"
#include "lights/light.h"
#include "cache.h"

class Material {
public:
	/*
	 * Apply the material shading properties to the hit object
	 */
	virtual Colorf shade(const Ray &r, const DifferentialGeometry &diff_geom,
		const std::vector<Light*> &lights) const = 0;
	/*
	 * Check if the material is reflective
	 */
	virtual bool is_reflective() const = 0;
	/*
	 * Get the reflective color of the object, is 0 if the
	 * object is not reflective
	 */
	virtual Colorf reflective() const = 0;
	/*
	 * Check if the object is transparent
	 */
	virtual bool is_transparent() const = 0;
	/*
	 * Get the object's absorption amount per unit t for transparency
	 */
	virtual Colorf absorbed() const = 0;
	/*
	 * Get the refractive index of the material,
	 * -1 is used to indicate the material isn't transparent
	 */
	virtual float refractive_idx() const = 0;
	/*
	 * Get the refractive color
	 */
	virtual Colorf refractive() const = 0;
};

typedef Cache<Material> MaterialCache;

#endif

