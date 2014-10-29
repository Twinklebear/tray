#include <array>
#include <atomic>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <memory>
#include <cstdio>
#include "linalg/util.h"
#include "film/render_target.h"

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
	: width(width), height(height), filter(std::move(f)), pixels(width * height),
	float_buf(width * height, std::numeric_limits<float>::infinity())
{
	//Pre-compute the filter table values
	for (int y = 0; y < FILTER_TABLE_SIZE; ++y){
		float fy = (y + .5f) * filter->h / FILTER_TABLE_SIZE;
		for (int x = 0; x < FILTER_TABLE_SIZE; ++x){
			float fx = (x + .5f) * filter->w / FILTER_TABLE_SIZE;
			filter_table[y * FILTER_TABLE_SIZE + x] = filter->weight(fx, fy);
		}
	}
}
void RenderTarget::write_pixel(float x, float y, const Colorf &c){
	//Compute the discrete pixel coordinates which the sample hits
	float img_x = x - 0.5f;
	float img_y = y - 0.5f;
	std::array<int, 2> x_range = {static_cast<int>(std::ceil(img_x - filter->w)),
		static_cast<int>(std::floor(img_x + filter->w))};
	std::array<int, 2> y_range = {static_cast<int>(std::ceil(img_y - filter->h)),
		static_cast<int>(std::floor(img_y + filter->h))};
	//Keep pixel coordinates in range and ignore degenerate ranges
	x_range[0] = std::max(x_range[0], 0);
	x_range[1] = std::min(x_range[1], static_cast<int>(width) - 1);
	y_range[0] = std::max(y_range[0], 0);
	y_range[1] = std::min(y_range[1], static_cast<int>(height) - 1);
	if (x_range[1] - x_range[0] < 0 || y_range[1] - y_range[0] < 0){
		return;
	}
	//Filter this sample to apply it to the pixels in the image affected by it
	for (int iy = y_range[0]; iy <= y_range[1]; ++iy){
		//Compute y location of this sample in the pre-computed filter values
		float fy = std::abs(iy - img_y) * filter->inv_h * FILTER_TABLE_SIZE;
		int fy_idx = std::min(static_cast<int>(fy), FILTER_TABLE_SIZE - 1);
		for (int ix = x_range[0]; ix <= x_range[1]; ++ix){
			//Compute x location of this sample in the pre-computed filter values
			float fx = std::abs(ix - img_x) * filter->inv_w * FILTER_TABLE_SIZE;
			int fx_idx = std::min(static_cast<int>(fx), FILTER_TABLE_SIZE - 1);
			float fweight = filter_table[fy_idx * FILTER_TABLE_SIZE + fx_idx];
			Pixel &p = pixels[iy * width + ix];
			atomic_addf(p.r, fweight * c.r);
			atomic_addf(p.g, fweight * c.g);
			atomic_addf(p.b, fweight * c.b);
			atomic_addf(p.weight, fweight);
		}
	}
}
void RenderTarget::write_float(size_t x, size_t y, float d){
	size_t i = clamp(y * width + x, size_t{0}, float_buf.size());
	float_buf[i] = d;
}
bool RenderTarget::save_image(const std::string &file) const {
	//Compute the correct image from the saved pixel data
	std::vector<Color24> img(width * height);
	get_colorbuf(img);
	return save_ppm(file, &img[0].r);
}
bool RenderTarget::save_depth(const std::string &file) const {
	std::vector<uint8_t> depth_norm = generate_depth_img();
	return save_pgm(file, depth_norm.data());
}
bool RenderTarget::save_heat(const std::string &file) const {
	std::vector<Color24> heat_map = generate_heat_img();
	return save_ppm(file, &heat_map[0].r);
}
size_t RenderTarget::get_width() const {
	return width;
}
size_t RenderTarget::get_height() const {
	return height;
}
void RenderTarget::get_colorbuf(std::vector<Color24> &img) const { 
	//Compute the correct image from the saved pixel data
	img.resize(width * height);
	for (size_t y = 0; y < height; ++y){
		for (size_t x = 0; x < width; ++x){
			const Pixel &p = pixels[y * width + x];
			float weight = p.weight.load(std::memory_order_consume);
			if (weight != 0){
				Colorf c{p.r.load(std::memory_order_consume),
					p.g.load(std::memory_order_consume),
					p.b.load(std::memory_order_consume)};
				c /= weight;
				c.normalize();
				img[y * width + x] = c;
			}
		}
	}
}
const std::vector<float>& RenderTarget::get_floatbuf() const {
	return float_buf;
}
std::vector<uint8_t> RenderTarget::generate_depth_img() const {
	std::vector<uint8_t> depth_norm(width * height);
	//Determine the min/max range of depth values so we can scale
	//them into 0-255 range
	float zmin = std::numeric_limits<float>::infinity();
	float zmax = std::numeric_limits<float>::min();
	for (const auto &f : float_buf){
		if (f == std::numeric_limits<float>::infinity()){
			continue;
		}
		if (f < zmin){
			zmin = f;
		}
		if (f > zmax){
			zmax = f;
		}
	}
	for (size_t i = 0; i < float_buf.size(); ++i){
		if (float_buf[i] == std::numeric_limits<float>::infinity()){
			depth_norm[i] = 0;
		}
		else {
			depth_norm[i] = static_cast<uint8_t>((zmax - float_buf[i]) / (zmax - zmin) * 255);
			depth_norm[i] = clamp(depth_norm[i], uint8_t{0}, uint8_t{255});
		}
	}
	return depth_norm;
}
std::vector<Color24> RenderTarget::generate_heat_img() const {
	//We use HSV to generate the heat map taking H = 240 as our min value (coolest)
	//and H = 0 as our max (hottest)
	std::vector<Color24> heat_map(width * height);
	float min = std::numeric_limits<float>::infinity();
	float max = std::numeric_limits<float>::min();
	for (const auto &f : float_buf){
		if (f == std::numeric_limits<float>::infinity()){
			continue;
		}
		if (f < min){
			min = f;
		}
		if (f > max){
			max = f;
		}
	}
	for (size_t i = 0; i < float_buf.size(); ++i){
		float hue = 240;
		if (float_buf[i] != std::numeric_limits<float>::infinity()){
			hue = (float_buf[i] - max) / (max - min) * -240.f;
			hue = clamp(hue, 0.f, 240.f);
		}
		hue /= 60;
		float x = 1 - std::abs(static_cast<int>(hue) % 2 - 1);
		if (hue >= 0 && hue < 1){
			heat_map[i] = Colorf{1, x, 0};
		}
		else if (hue < 2){
			heat_map[i] = Colorf{x, 1, 0};
		}
		else if (hue < 3){
			heat_map[i] = Colorf{0, 1, x};
		}
		else if (hue < 4){
			heat_map[i] = Colorf{0, x, 1};
		}
		else if (hue < 5){
			heat_map[i] = Colorf{x, 0, 1};
		}
		else if (hue < 6){
			heat_map[i] = Colorf{1, 0, x};
		}
	}
	return heat_map;
}
bool RenderTarget::save_ppm(const std::string &file, const uint8_t *data) const {
	FILE *fp = fopen(file.c_str(), "wb");
	if (!fp){
		std::cerr << "RenderTarget::save_ppm Error: failed to open file " << file << std::endl;
		return false;
	}
	fprintf(fp, "P6\n%d %d\n255\n", static_cast<int>(width), static_cast<int>(height));
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
	fprintf(fp, "P5\n%d %d\n255\n", static_cast<int>(width), static_cast<int>(height));
	fwrite(data, 1, width * height, fp);
	fclose(fp);
	return true;
}

