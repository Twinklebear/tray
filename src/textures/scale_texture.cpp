#include "textures/texture.h"
#include "textures/scale_texture.h"

ScaleTexture::ScaleTexture(const Texture &a, const Texture &b) : a(a), b(b){}
Colorf ScaleTexture::sample(const DifferentialGeometry &dg) const {
	return a.sample(dg) * b.sample(dg);
}
Colorf ScaleTexture::sample(const TextureSample &sample) const {
	return a.sample(sample) * b.sample(sample);
}

