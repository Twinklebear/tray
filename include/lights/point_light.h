#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "render/color.h"
#include "linalg/point.h"
#include "light.h"

class PointLight : public Light {
	Colorf color;
	Point position;

public:
	PointLight(const Colorf &color, const Point &pos);
	Colorf illuminate(const Point &p) const override;
	Vector direction(const Point &p) const override;
	bool is_ambient() const override;
};

#endif

