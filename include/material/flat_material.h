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
	Colorf shade(const Ray &r, const DifferentialGeometry &diff_geom,
		const std::vector<Light*> &lights) const override;
	bool is_reflective() const override;
	Colorf reflective() const override;
	bool is_transparent() const override;
	Colorf absorbed() const override;
	float refractive_idx() const override;
	Colorf refractive() const override;
};

#endif

