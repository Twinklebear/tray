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
	LIGHT type() const override;
};

#endif

