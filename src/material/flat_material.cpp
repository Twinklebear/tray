#include "material/flat_material.h"

FlatMaterial::FlatMaterial(const Colorf &color) : color(color) {}
Colorf FlatMaterial::shade(const Ray &r, const HitInfo &hitinfo) const {
	return color;
}

