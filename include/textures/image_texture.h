#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include <string>
#include <vector>
#include <memory>
#include "texture.h"
#include "texture_mapping.h"
#include "mipmap.h"

/*
 * A 2D image to be used as a texture
 * If the texture has one color component the color returned when sampling
 * will be RRR.
 * If two components RG0 is returned, for three RGB is returned
 */
class ImageTexture : public Texture {
	std::unique_ptr<TextureMapping> mapping;
	//Image width, height and number of components per pixel
	int width, height, ncomp;
	MipMap mipmap;

public:
	/*
	 * Load the image texture from an image file, mapping controls
	 * how u,v coordinates are mapped to texture s,t coordinates
	 */
	ImageTexture(const std::string &file, std::unique_ptr<TextureMapping> mapping,
		WRAP_MODE wrap_mode = WRAP_MODE::REPEAT);
	/*
	 * Sample the texture color for the piece of geometry being textured
	 */
	Colorf sample(const DifferentialGeometry &dg) const override;
	/*
	 * Sometimes we want to re-use a texture but through a different mapping
	 * This method samples the texture directly using the texture sample passed in
	 */
	Colorf sample(const TextureSample &sample) const override;

private:
	/*
	 * Load a texture from an image file and return the status of the load
	 */
	bool load_image(const std::string &file, std::vector<uint8_t> &texels);
	/*
	 * Load a texture from a ppm file and return the status of the load
	 */
	bool load_ppm(const std::string &file, std::vector<uint8_t> &texels);
	/*
	 * Load a texture using stb_image and return the status of the load
	 */
	bool load_stb(const std::string &file, std::vector<uint8_t> &texels);
};

#endif

