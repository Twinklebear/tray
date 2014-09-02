#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <string>
#include <vector>

//Since we fwrite this struct directly and PPM only takes RGB (24 bits)
//we can't allow any padding to be added onto the end
#pragma pack(1)
struct Color {
	uint8_t r, g, b;

	Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0);
};

/*
 * The render target where pixel data is stored for the rendered scene
 * along with for some reason a depth buffer is required for proj1?
 */
class RenderTarget {
	size_t width, height;
	std::vector<Color> color;
	std::vector<float> depth;

public:
	/*
	 * Create a render target with width * height pixels
	 */
	RenderTarget(size_t width, size_t height);
	/*
	 * Write a color value to the image at pixel(x, y)
	 */
	void write_pixel(size_t x, size_t y, const Color &c);
	/*
	 * Write a depth value to the depth buffer at pixel(x, y)
	 */
	void write_depth(size_t x, size_t y, float d);
	//Save the image or depth buffer to the desired file
	bool save_image(const std::string &file) const;
	bool save_depth(const std::string &file) const;
	size_t get_width() const;
	size_t get_height() const;
	const std::vector<Color>& get_colorbuf() const;
	const std::vector<float>& get_depthbuf() const;
	/*
	 * Generate a normalized luminosity image of the depth buffer
	 */
	std::vector<uint8_t> generate_depth_img() const;

private:
	/*
	 * Save color data as a PPM image to the file, data should be
	 * RGB8 data and have width * height elements
	 */
	bool save_ppm(const std::string &file, const uint8_t *data) const;
	/*
	 * Save data as a PGM image to the file, data should
	 * be R8 data and have width * height elements, image will be grayscale
	 */
	bool save_pgm(const std::string &file, const uint8_t *data) const;
};

#endif

