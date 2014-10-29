#ifndef MIPMAP_H
#define MIPMAP_H

#include <vector>
#include "film/color.h"
#include "texture_mapping.h"

/*
 * Enum to select the wrapping mode for out of bound texture coordinates
 * REPEAT: coords will be wrapped around the image via abs(x) % dim
 * CLAMP: coords will be clamped into range via clamp(x, 0, dim)
 * BLACK: out of bounds coords return black
 */
enum class WRAP_MODE {REPEAT, CLAMP, BLACK};

/*
 * Stores a mipmap image pyramid and can perform trilinear and EWA filtering
 * of/between the levels in the pyramid
 */
class MipMap {
	WRAP_MODE wrap_mode;
	int width, height, ncomp;

	//A level in the mip map pyramid
	struct MipLevel {
		int width, height;
		std::vector<uint8_t> texels;

		MipLevel(int width, int height, const std::vector<uint8_t> &texels = std::vector<uint8_t>{});
	};
	std::vector<MipLevel> pyramid;
	const static int WEIGHT_LUT_SIZE = 128;
	static std::array<float, WEIGHT_LUT_SIZE> weight_table;

public:
	/*
	 * Create an empty mipmap pyramid
	 */
	MipMap();
	/*
	 * Construct the mipmap pyramid for the image passed
	 * pixels: the image
	 * width, height: image dimensions
	 * ncomp: the number of components per pixel
	 * wrap_mode: the desired wrapping mode, defaults to repeat
	 */
	MipMap(const std::vector<uint8_t> &texels, int width, int height, int ncomp,
		WRAP_MODE wrap_mode = WRAP_MODE::REPEAT);
	/*
	 * Get the color of a texel at some level of the image
	 */
	Colorf texel(int level, int s, int t) const;
	/*
	 * Perform image filtering over the texels and pyramid levels to compute the
	 * color value for the sample
	 */
	Colorf sample(const TextureSample &samp) const;

private:
	/*
	 * Perform trilinear filtering selecting the mipmap levels based
	 * on the filter width
	 */
	Colorf sample_trilinear(const TextureSample &samp, float w) const;
	/*
	 * Compute the bilinearly interpolated color at the sample positions
	 * using a triangle filter
	 */
	Colorf triangle_filter(int lvl, float s, float t) const;
	/*
	 * Perform elliptically weighted average filtering to find the color
	 * for the sample
	 */
	Colorf sample_ewa(const TextureSample &samp, std::array<float, 2> ds,
		std::array<float, 2> dt) const;
	/*
	 * Compute the elliptically weighted average color at the sample position
	 */
	Colorf ewa_filter(int lvl, float s, float t, std::array<float, 2> ds,
		std::array<float, 2> dt) const;
	/*
	 * Initialize the precomputed weight lookup table for EWA filtering used
	 * by all the mip-maps
	 */
	static void init_weight_table();
};

#endif

