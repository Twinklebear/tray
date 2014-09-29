#include <memory>
#include "textures/texture_mapping.h"
#include "textures/uv_texture.h"

UVTexture::UVTexture(std::unique_ptr<TextureMapping> mapping) : mapping(std::move(mapping)){}
Colorf UVTexture::sample(const DifferentialGeometry &dg) const {
	TextureSample sample = mapping->map(dg);
	return Colorf{sample.s, sample.t, 0};
}

