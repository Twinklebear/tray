#include <cstdio>
#include <string>
#include <vector>
#include <memory>
#include "textures/texture.h"
#include "textures/texture_mapping.h"
#include "textures/image_texture.h"

ImageTexture::ImageTexture(const std::string &file, std::unique_ptr<TextureMapping> mapping)
	: mapping(mapping), width(0), height(0), ncomp(0)
{
	if (!load_image(file)){
		std::cout << "ImageTexture Error: could not load " << file << std::endl;
	}
}
Colorf ImageTexture::sample(const DifferentialGeometry &dg) const {
	if (width == 0){
		return Colorf{0, 0, 0};
	}
}
bool ImageTexture::load_image(const std::string &file){
	std::cout << "file ext: " << file.substr(file.rfind(".")) << std::endl;
	if (file.substr(file.rfind(".")) == ".ppm"){
		return load_ppm(file);
	}
	return load_stb(file);
}
bool ImageTexture::load_ppm(const std::string &file){
}
bool ImageTexture::load_stb(const std::string &file){
}

