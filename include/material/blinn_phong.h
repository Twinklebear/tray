#ifndef BLINN_PHONG_H
#define BLINN_PHONG_H

#include "material.h"

class BlinnPhong : public Material {
	Colorf diffuse, specular, reflection, refraction, absorption;
	float gloss, refr_index;

public:
	/*
	 * Create a Blinn-Phong material with the desired color, specular
	 * and glossiness
	 */
	BlinnPhong(const Colorf &diffuse, const Colorf &specular, float gloss, const Colorf &reflection,
		const Colorf &refraction, const Colorf &absorption, float refr_index);
	/*
	 * Apply the BlinnPhong material shading model to the point
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

