#include "lights/light.h"
#include "textures/texture.h"
#include "material/flat_material.h"

FlatMaterial::FlatMaterial(const Texture *texture) : texture(texture){}
Colorf FlatMaterial::shade(const Ray&, const DifferentialGeometry &diff_geom, const Light&) const {
	return texture->sample(diff_geom);
}
bool FlatMaterial::is_reflective() const {
	return false;
}
Colorf FlatMaterial::reflective(const DifferentialGeometry&) const {
	return Colorf{0, 0, 0};
}
bool FlatMaterial::is_transparent() const {
	return false;
}
Colorf FlatMaterial::absorbed(const DifferentialGeometry&) const {
	return Colorf{0, 0, 0};
}
float FlatMaterial::refractive_idx() const {
	return -1;
}
Colorf FlatMaterial::refractive(const DifferentialGeometry&) const {
	return Colorf{0, 0, 0};
}

