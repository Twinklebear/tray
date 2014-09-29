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
#include "textures/image_texture.h"

ImageTexture::ImageTexture(const std::string &file, std::unique_ptr<TextureMapping> mapping, WRAP_MODE wrap_mode)
	: mapping(std::move(mapping)), wrap_mode(wrap_mode), width(0), height(0), ncomp(0)
{
	if (!load_image(file)){
		std::cout << "ImageTexture Error: could not load " << file << std::endl;
	}
}
Colorf ImageTexture::sample(const DifferentialGeometry &dg) const {
	if (width == 0){
		return Colorf{0, 0, 0};
	}
	static float inv = 1.f / 255.f;
	TextureSample sample = mapping->map(dg);
	//TODO: No filtering for now
	int ts = static_cast<int>(sample.s * width);
	int tt = static_cast<int>(sample.t * height);
	switch (wrap_mode){
		case WRAP_MODE::REPEAT:
			ts = std::abs(ts) % width;
			tt = std::abs(tt) % height;
			break;
		case WRAP_MODE::CLAMP:
			ts = clamp(ts, 0, width - 1);
			tt = clamp(tt, 0, height - 1);
			break;
		case WRAP_MODE::BLACK:
			if (ts < 0 || ts >= width || tt < 0 || tt >= height){
				return Colorf{0, 0, 0};
			}
	}
	switch (ncomp){
		case 1:
			return Colorf{pixels[tt * width * ncomp + ts * ncomp] * inv};
		case 2:
			return Colorf{pixels[tt * width * ncomp + ts * ncomp] * inv,
				pixels[tt * width * ncomp + ts * ncomp + 1] * inv, 0};
		default:
			return Colorf{pixels[tt * width * ncomp + ts * ncomp] * inv,
				pixels[tt * width * ncomp + ts * ncomp + 1] * inv,
				pixels[tt * width * ncomp + ts * ncomp + 2] * inv};
	}
}
bool ImageTexture::load_image(const std::string &file){
	if (file.substr(file.rfind(".")) == ".ppm"){
		return load_ppm(file);
	}
	return load_stb(file);
}
bool ImageTexture::load_ppm(const std::string &file){
	std::ifstream f{file.c_str(), std::ios::in | std::ios::binary};
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
	pixels.resize(width * height * ncomp);
	if (!f.read(reinterpret_cast<char*>(pixels.data()), width * height * ncomp)){
		return false;
	}
	return true;
}
bool ImageTexture::load_stb(const std::string &file){
	uint8_t *img = stbi_load(file.c_str(), &width, &height, &ncomp, 0);
	if (!img){
		return false;
	}
	pixels.reserve(width * height * ncomp);
	pixels.insert(pixels.begin(), img, img + width * height * ncomp);
	stbi_image_free(img);
	return true;
}

