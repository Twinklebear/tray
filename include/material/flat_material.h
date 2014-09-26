#ifndef FLAT_MATERIAL_H
#define FLAT_MATERIAL_H

#include "material.h"
#include "textures/texture.h"

class FlatMaterial : public Material {
	//Non-owning reference the texture we're drawing
	const Texture* texture;

public:
	/*
	 * Create a basic flat material that will simply return
	 * the color set
	 */
	FlatMaterial(const Texture *texture);
	/*
	 * "Shade" the object, just returns the color set
	 */
	Colorf shade(const Ray &r, const DifferentialGeometry &diff_geom,
		const std::vector<Light*> &lights) const override;
	bool is_reflective() const override;
	Colorf reflective(const DifferentialGeometry &dg) const override;
	bool is_transparent() const override;
	Colorf absorbed(const DifferentialGeometry &dg) const override;
	float refractive_idx() const override;
	Colorf refractive(const DifferentialGeometry &dg) const override;
};

#endif

