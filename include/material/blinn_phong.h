#ifndef BLINN_PHONG_H
#define BLINN_PHONG_H

#include "linalg/point.h"
#include "material.h"

class BlinnPhong : public Material {
	Point diffuse, specular;
	float gloss;

public:
	/*
	 * Create a Blinn-Phong material with the desired color, specular
	 * and glossiness
	 */
	BlinnPhong(const Point &diffuse, const Point &specular, float gloss);
	/*
	 * Apply the BlinnPhong material shadin
	 */
	Point shade(const Ray &ray) const override;
};

#endif

