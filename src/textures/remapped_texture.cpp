#include <memory>
#include "textures/texture.h"
#include "textures/texture_mapping.h"
#include "textures/remapped_texture.h"

RemappedTexture::RemappedTexture(const Texture &texture, std::unique_ptr<TextureMapping> mapping)
	: texture(texture), mapping(std::move(mapping))
{}
Colorf RemappedTexture::sample(const DifferentialGeometry &dg) const {
	return sample(mapping->map(dg));
}
Colorf RemappedTexture::sample(const TextureSample &sample) const {
	return texture.sample(sample);
}

