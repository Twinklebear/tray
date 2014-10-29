#include "lights/light.h"
#include "lights/ambient_light.h"

AmbientLight::AmbientLight(const Colorf &color) : color(color) {}
Colorf AmbientLight::illuminate(const Point&) const {
	return color;
}
Vector AmbientLight::direction(const Point&) const {
	return Vector{};
}
LIGHT AmbientLight::type() const {
	return LIGHT::AMBIENT;
}

