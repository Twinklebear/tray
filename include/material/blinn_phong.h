#ifndef BLINN_PHONG_H
#define BLINN_PHONG_H

#include "material.h"

class BlinnPhong : public Material {
	Colorf diffuse, specular;
	float gloss;

public:
	/*
	 * Create a Blinn-Phong material with the desired color, specular
	 * and glossiness
	 */
	BlinnPhong(const Colorf &diffuse, const Colorf &specular, float gloss);
	/*
	 * Apply the BlinnPhong material shading model to the point
	 */
	Colorf shade(const Ray &r, const HitInfo &hitinfo) const override;
};

#endif

