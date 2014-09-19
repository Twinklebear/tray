#include <array>
#include <atomic>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <memory>
#include <cstdio>
#include "linalg/util.h"
#include "render/render_target.h"

/*
 * Perform an atomic addition to the float via spin-locking
 * on compare_exchange_weak. Memory ordering is release on write
 * consume on read
 */
static float atomic_addf(std::atomic<float> &f, float d){
	float old = f.load(std::memory_order_consume);
	float desired = old + d;
	while (!f.compare_exchange_weak(old, desired,
		std::memory_order_release, std::memory_order_consume))
	{
		desired = old + d;
	}
	return desired;
}

Pixel::Pixel() : r(0), g(0), b(0), weight(0){}
Pixel::Pixel(const Pixel &p) : r(p.r.load(std::memory_order_consume)), g(p.g.load(std::memory_order_consume)),
	b(p.b.load(std::memory_order_consume)), weight(p.weight.load(std::memory_order_consume))
{}

RenderTarget::RenderTarget(size_t width, size_t height, std::unique_ptr<Filter> f)
	: width(width), height(height), filter(std::move(f)), color(width * height),
	pixels(width * height), depth(width * height, std::numeric_limits<float>::max())
{}
void RenderTarget::write_pixel(float x, float y, const Colorf &c){
	//Compute the discrete pixel coordinates which the sample hits
	float img_x = x - 0.5f;
	float img_y = y - 0.5f;
	//Using the defaults for the box filter in PBR, TODO take a filter param
	//and use it
	std::array<int, 2> x_range = {static_cast<int>(std::ceil(img_x - 0.5f)),
		static_cast<int>(std::floor(img_x + 0.5f))};
	std::array<int, 2> y_range = {static_cast<int>(std::ceil(img_y - 0.5f)),
		static_cast<int>(std::floor(img_y + 0.5f))};
	//Keep pixel coordinates in range and ignore degenerate ranges
	x_range[0] = std::max(x_range[0], 0);
	x_range[1] = std::min(x_range[1], static_cast<int>(width) - 1);
	y_range[0] = std::max(y_range[0], 0);
	y_range[1] = std::min(y_range[1], static_cast<int>(height) - 1);
	if (x_range[1] - x_range[0] < 0 || y_range[1] - y_range[0] < 0){
		return;
	}
	color[(int)y * width + (int)x] = static_cast<Color24>(c);
	//Filter this sample to apply it to the pixels in the image affected by it
	//TODO: check this condition based on the filter width
	for (int iy = y_range[0]; iy <= y_range[1]; ++iy){
		for (int ix = x_range[0]; ix <= x_range[1]; ++ix){
			//Box filter is always 1 in its extent
			float filter = 1;
			Pixel &p = pixels[iy * width + x];
			atomic_addf(p.r, filter * c.r);
			atomic_addf(p.g, filter * c.g);
			atomic_addf(p.b, filter * c.b);
			atomic_addf(p.weight, filter);
		}
	}
	color[y_range[0] * width + x_range[0]] = c;
}
void RenderTarget::write_depth(size_t x, size_t y, float d){
	depth[y * width + x] = d;
}
bool RenderTarget::save_image(const std::string &file) const {
	//Compute the correct image from the saved pixel data
	std::vector<Color24> img(width * height);
	for (size_t y = 0; y < height; ++y){
		for (size_t x = 0; x < width; ++x){
			const Pixel &p = pixels[y * width + x];
			float weight = p.weight.load(std::memory_order_consume);
			if (weight != 0){
				Colorf c{p.r.load(std::memory_order_consume),
					p.g.load(std::memory_order_consume),
					p.b.load(std::memory_order_consume)};
				img[y * width + x] = c / weight;
			}
		}
	}
	return save_ppm(file, &img[0].r);
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
const std::vector<Color24>& RenderTarget::get_colorbuf() const { 
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
		if (f == std::numeric_limits<float>::max()){
			continue;
		}
		if (f < zmin){
			zmin = f;
		}
		if (f > zmax){
			zmax = f;
		}
	}
	for (size_t i = 0; i < depth.size(); ++i){
		if (depth[i] == std::numeric_limits<float>::max()){
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

