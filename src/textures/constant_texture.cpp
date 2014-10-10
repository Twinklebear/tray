#include "textures/constant_texture.h"

ConstantTexture::ConstantTexture(const Colorf &color) : color(color){}
Colorf ConstantTexture::sample(const DifferentialGeometry &) const {
	return color;
}
Colorf ConstantTexture::sample(const TextureSample &sample) const {
	return color;
}

