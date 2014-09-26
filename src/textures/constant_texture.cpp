#include "textures/constant_texture.h"

ConstantTexture::ConstantTexture(const Colorf &color) : color(color){}
Colorf ConstantTexture::sample(const DifferentialGeometry &dg) const {
	return color;
}

