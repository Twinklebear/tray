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
	: width(width), height(height), filter(std::move(f)), pixels(width * height)
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
bool RenderTarget::save_image(const std::string &file) const {
	//Compute the correct image from the saved pixel data
	std::vector<Color24> img(width * height);
	get_colorbuf(img);
	return save_ppm(file, &img[0].r);
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
				img[y * width + x] = c.to_sRGB();
			}
		}
	}
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

