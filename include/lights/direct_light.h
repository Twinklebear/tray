#ifndef DIRECT_LIGHT_H
#define DIRECT_LIGHT_H

#include "render/color.h"
#include "linalg/vector.h"
#include "light.h"

class DirectLight : public Light {
	Colorf color;
	Vector dir;

public:
	DirectLight(const Colorf &color, const Vector &dir);
	Colorf illuminate(const Point &p) const override;
	Vector direction(const Point &p) const override;
	bool is_ambient() const override;
};

#endif

