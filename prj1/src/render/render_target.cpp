#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <cstdio>
#include "linalg/util.h"
#include "render/render_target.h"

Color::Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

RenderTarget::RenderTarget(size_t width, size_t height) : width(width), height(height),
	color(width * height), depth(width * height, std::numeric_limits<float>::max())
{}
void RenderTarget::write_pixel(size_t x, size_t y, const Color &c){
	color[y * width + x] = c;
}
void RenderTarget::write_depth(size_t x, size_t y, float d){
	depth[y * width + x] = d;
}
bool RenderTarget::save_image(const std::string &file) const {
	return save_ppm(file, &color[0].r);
}
bool RenderTarget::save_depth(const std::string &file) const {
	std::vector<uint8_t> depth_norm = generate_depth_img();
	return save_pgm(file, depth_norm.data());
}
size_t RenderTarget::get_width() const {
	return width;
}
size_t RenderTarget::get_height() const {
	return height;
}
const std::vector<Color>& RenderTarget::get_colorbuf() const { 
	return color;
}
const std::vector<float>& RenderTarget::get_depthbuf() const {
	return depth;
}
std::vector<uint8_t> RenderTarget::generate_depth_img() const {
	std::vector<uint8_t> depth_norm(width * height);
	//Determine the min/max range of depth values so we can scale
	//them into 0-255 range
	float zmin = std::numeric_limits<float>::max();
	float zmax = std::numeric_limits<float>::min();
	for (const float &f : depth){
		if (f < zmin){
			zmin = f;
		}
		if (f > zmax){
			zmax = f;
		}
	}
	for (size_t i = 0; i < depth.size(); ++i){
		if (depth[i] > zmax){
			depth_norm[i] = 0;
		}
		else {
			depth_norm[i] = static_cast<uint8_t>((zmax - depth[i]) / (zmax - zmin) * 255);
			depth_norm[i] = clamp(depth_norm[i], uint8_t{0}, uint8_t{255});
		}
	}
	return depth_norm;
}
bool RenderTarget::save_ppm(const std::string &file, const uint8_t *data) const {
	FILE *fp = fopen(file.c_str(), "wb");
	if (!fp){
		std::cerr << "RenderTarget::save_ppm Error: failed to open file " << file << std::endl;
		return false;
	}
	fprintf(fp, "P6\n%d %d\n255\n", width, height);
	fwrite(data, 1, 3 * width * height, fp);
	fclose(fp);
	return true;
}
bool RenderTarget::save_pgm(const std::string &file, const uint8_t * data) const {
	FILE *fp = fopen(file.c_str(), "wb");
	if (!fp){
		std::cerr << "RenderTarget::save_pgm Error: failed to open file " << file << std::endl;
		return false;
	}
	fprintf(fp, "P5\n%d %d\n255\n", width, height);
	fwrite(data, 1, width * height, fp);
	fclose(fp);
	return true;
}


