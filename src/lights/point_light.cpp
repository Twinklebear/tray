#include "linalg/point.h"
#include "lights/point_light.h"

PointLight::PointLight(const Colorf &color, const Point &pos)
	: color(color), position(pos)
{}
Colorf PointLight::illuminate(const Point &p) const {
	return color;
}
Vector PointLight::direction(const Point &p) const {
	return p - position;
}
LIGHT PointLight::type() const {
	return LIGHT::POINT;
}

