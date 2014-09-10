#include "lights/light.h"
#include "material/flat_material.h"

FlatMaterial::FlatMaterial(const Colorf &color) : color(color) {}
Colorf FlatMaterial::shade(const Ray &r, const HitInfo &hitinfo, const std::vector<Light*> &lights) const {
	return color;
}
bool FlatMaterial::is_reflective() const {
	return false;
}
Colorf FlatMaterial::reflective() const {
	return Colorf{0, 0, 0};
}
bool FlatMaterial::is_transparent() const {
	return false;
}
float FlatMaterial::refractive_idx() const {
	return -1;
}
Colorf FlatMaterial::refractive() const {
	return Colorf{0, 0, 0};
}

