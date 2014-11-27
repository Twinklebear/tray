#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <string>
#include <vector>
#include <atomic>
#include <memory>
#include "color.h"
#include "filters/filter.h"

const int FILTER_TABLE_SIZE = 16;

/*
 * A pixel stored in the image being rendered to track pixel
 * luminance and weight for reconstruction
 * Because we need to deal with multi-thread synchronization the
 * rgb values and weights are stored as atomics
 */
struct Pixel {
	std::atomic<float> r, g, b, weight;

	Pixel();
	Pixel(const Pixel &p);
};

/*
 * The render target where pixel data is stored for the rendered scene
 * along with for some reason a depth buffer is required for proj1?
 */
class RenderTarget {
	size_t width, height;
	std::unique_ptr<Filter> filter;
	std::vector<Pixel> pixels;
	//Pre-computed filter values to save time when storing pixels
	std::array<float, FILTER_TABLE_SIZE * FILTER_TABLE_SIZE> filter_table;

public:
	/*
	 * Create a render target with width * height pixels that will
	 * reconstruct the image from the samples using the desired filter
	 * Default is a box filter with single pixel extent
	 */
	RenderTarget(size_t width, size_t height, std::unique_ptr<Filter> f);
	/*
	 * Write a color value to the image at pixel(x, y)
	 */
	void write_pixel(float x, float y, const Colorf &c);
	/*
	 * Write a float value to the float buffer at pixel(x, y)
	 */
	void write_float(size_t x, size_t y, float d);
	//Save the image or depth buffer to the desired file
	bool save_image(const std::string &file) const;
	size_t get_width() const;
	size_t get_height() const;
	/*
	 * Get a snapshot of the color buffer at the moment
	 * stored in img
	 */
	void get_colorbuf(std::vector<Color24> &img) const;

private:
	/*
	 * Save color data as a PPM image to the file, data should be
	 * RGB8 data and have width * height elements
	 */
	bool save_ppm(const std::string &file, const uint8_t *data) const;
};

#endif

