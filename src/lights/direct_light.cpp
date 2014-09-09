#include "render/color.h"
#include "linalg/vector.h"
#include "lights/direct_light.h"

DirectLight::DirectLight(const Colorf &color, const Vector &dir)
	: color(color), dir(dir.normalized())
{}
Colorf DirectLight::illuminate(const Point &p) const {
	return color;
}
Vector DirectLight::direction(const Point &p) const {
	return dir;
}
LIGHT DirectLight::type() const {
	return LIGHT::DIRECT;
}

