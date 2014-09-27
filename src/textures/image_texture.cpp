#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "textures/texture.h"
#include "textures/texture_mapping.h"
#include "textures/image_texture.h"

ImageTexture::ImageTexture(const std::string &file, std::unique_ptr<TextureMapping> mapping)
	: mapping(std::move(mapping)), width(0), height(0), ncomp(0)
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
	//Handle # of components?
	return Colorf{pixels[tt * width * ncomp + ts * ncomp] * inv,
		pixels[tt * width * ncomp + ts * ncomp + 1] * inv,
		pixels[tt * width * ncomp + ts * ncomp + 2] * inv};
}
bool ImageTexture::load_image(const std::string &file){
	if (file.substr(file.rfind(".")) == ".ppm"){
		return load_ppm(file);
	}
	return load_stb(file);
}
bool ImageTexture::load_ppm(const std::string &file){
	return false;
}
bool ImageTexture::load_stb(const std::string &file){
	uint8_t *img = stbi_load(file.c_str(), &width, &height, &ncomp, 0);
	if (!img){
		return false;
	}
	//Perform y-flip of image so we'll have it right side up in our mapping
	for (int i = 0; i < height / 2; ++i){
		std::swap_ranges(&img[i * width * ncomp], &img[(i + 1) * width * ncomp],
			&img[(height - i - 1) * width * ncomp]);
	}
	pixels.reserve(width * height * ncomp);
	pixels.insert(pixels.begin(), img, img + width * height * ncomp);
	stbi_image_free(img);
	return true;
}

