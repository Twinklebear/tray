#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "linalg/util.h"
#include "textures/texture.h"
#include "textures/texture_mapping.h"
#include "textures/mipmap.h"
#include "textures/image_texture.h"

ImageTexture::ImageTexture(const std::string &file, std::unique_ptr<TextureMapping> mapping, WRAP_MODE wrap_mode)
	: mapping(std::move(mapping)), width(0), height(0), ncomp(0)
{
	std::vector<uint8_t> texels;
	if (!load_image(file, texels)){
		std::cout << "ImageTexture Error: could not load " << file << std::endl;
	}
	else {
		mipmap = MipMap(texels, width, height, ncomp, wrap_mode);
	}
}
Colorf ImageTexture::sample(const DifferentialGeometry &dg) const {
	return sample(mapping->map(dg));
}
Colorf ImageTexture::sample(const TextureSample &sample) const {
	return mipmap.sample(sample);
}
bool ImageTexture::load_image(const std::string &file, std::vector<uint8_t> &texels){
	if (file.substr(file.rfind(".")) == ".ppm"){
		return load_ppm(file, texels);
	}
	return load_stb(file, texels);
}
bool ImageTexture::load_ppm(const std::string &file, std::vector<uint8_t> &texels){
	std::ifstream f{file, std::ios::in | std::ios::binary};
	if (!f){
		return false;
	}
	std::string line;
	if (!std::getline(f, line) || line != "P6"){
		return false;
	}
	//Skip comments
	while (std::getline(f, line) && line[0] == '#');

	size_t sep = line.find(' ');
	width = std::stoi(line.substr(0, sep));
	height = std::stoi(line.substr(sep + 1));
	//Read the max val, but we know it's 255 so ignore it
	std::getline(f, line);
	ncomp = 3;
	texels.resize(width * height * ncomp);
	if (!f.read(reinterpret_cast<char*>(texels.data()), width * height * ncomp)){
		return false;
	}
	return true;
}
bool ImageTexture::load_stb(const std::string &file, std::vector<uint8_t> &texels){
	uint8_t *img = stbi_load(file.c_str(), &width, &height, &ncomp, 0);
	if (!img){
		return false;
	}
	texels.reserve(width * height * ncomp);
	texels.insert(texels.begin(), img, img + width * height * ncomp);
	stbi_image_free(img);
	return true;
}

