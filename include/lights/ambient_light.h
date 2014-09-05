#ifndef AMBIENT_LIGHT_H
#define AMBIENT_LIGHT_H

#include "light.h"

/*
 * Basic ambient light
 */
class AmbientLight : public Light {
	Colorf color;

public:
	AmbientLight(const Colorf &color);
	Colorf illuminate(const Point &p) const override;
	Vector direction(const Point &p) const override;
	bool is_ambient() const override;
};

#endif

