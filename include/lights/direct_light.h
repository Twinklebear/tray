#ifndef DIRECT_LIGHT_H
#define DIRECT_LIGHT_H

#include "film/color.h"
#include "linalg/vector.h"
#include "light.h"

class DirectLight : public Light {
	Colorf color;
	Vector dir;

public:
	DirectLight(const Colorf &color, const Vector &dir);
	Colorf illuminate(const Point &p) const override;
	Vector direction(const Point &p) const override;
	LIGHT type() const override;
};

#endif

