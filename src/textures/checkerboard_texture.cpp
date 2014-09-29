#include <memory>
#include "textures/texture.h"
#include "textures/texture_mapping.h"
#include "textures/checkerboard_texture.h"

CheckerboardTexture::CheckerboardTexture(const Colorf &a, const Colorf &b,
	std::unique_ptr<TextureMapping> mapping)
	: mapping(std::move(mapping)), a(a), b(b)
{}
Colorf CheckerboardTexture::sample(const DifferentialGeometry &dg) const {
	TextureSample sample = mapping->map(dg);
	//TODO: No filtering at the moment
	/*
	std::cout << "sample s = " << sample.s << ", t = " << sample.t
		<< "\nchecker pos: " << static_cast<int>(sample.s) + static_cast<int>(sample.t)
		<< std::endl;
		*/
	if ((static_cast<int>(sample.s) + static_cast<int>(sample.t)) % 2 == 0){
		return a;
	}
	return b;
}

