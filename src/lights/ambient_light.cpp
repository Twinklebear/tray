#include "lights/light.h"
#include "lights/ambient_light.h"

AmbientLight::AmbientLight(const Colorf &color) : color(color) {}
Colorf AmbientLight::illuminate(const Point &p) const {
	return color;
}
Vector AmbientLight::direction(const Point &p) const {
	return Vector{};
}
LIGHT AmbientLight::type() const {
	return LIGHT::AMBIENT;
}

