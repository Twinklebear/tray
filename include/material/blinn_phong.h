#ifndef BLINN_PHONG_H
#define BLINN_PHONG_H

#include "material.h"
#include "textures/texture.h"

class BlinnPhong : public Material {
	const Texture *diffuse, *specular, *reflection, *refraction, *absorption;
	float gloss, refr_index;

public:
	/*
	 * Create a Blinn-Phong material with the desired color, specular
	 * and glossiness
	 */
	BlinnPhong(const Texture *diffuse, const Texture *specular, float gloss, const Texture *reflection,
		const Texture *refraction, const Texture *absorption, float refr_index);
	/*
	 * Apply the BlinnPhong material shading model to the point
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

