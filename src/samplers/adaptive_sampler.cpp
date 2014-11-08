#include <iostream>
#include <array>
#include <chrono>
#include <vector>
#include <memory>
#include <random>
#include "linalg/util.h"
#include "samplers/ld_sampler.h"
#include "samplers/adaptive_sampler.h"

AdaptiveSampler::AdaptiveSampler(int x_start, int x_end, int y_start, int y_end, int min_sp, int max_sp)
	: Sampler{x_start, x_end, y_start, y_end}, min_spp(round_up_pow2(min_sp)), max_spp(round_up_pow2(max_sp)),
	supersample_px(min_spp)
{
	if (min_sp % 2 != 0){
		std::cout << "Warning: AdaptiveSampler requires power of 2 samples per pixel."
			<< " Rounded min_spp up to " << min_spp << std::endl;
	}
	if (max_sp % 2 != 0){
		std::cout << "Warning: AdaptiveSampler requires power of 2 samples per pixel."
			<< " Rounded max_spp up to " << max_spp << std::endl;
	}
}
void AdaptiveSampler::get_samples(std::vector<Sample> &samples){
	samples.clear();
	if (supersample_px == min_spp && !has_samples()){
		return;
	}
	int spp = supersample_px;
	samples.resize(spp);
	std::vector<std::array<float, 2>> pos(spp), lens(spp);
	std::vector<float> time(spp);
	get_samples(pos.data(), pos.size());
	get_samples(lens.data(), lens.size());
	get_samples(time.data(), time.size());
	auto p = pos.begin();
	auto l = lens.begin();
	auto t = time.begin();
	auto s = samples.begin();
	for (; s != samples.end(); ++p, ++l, ++t, ++s){
		*s = Sample{*p, *l, *t};
	}
	for (auto &s : samples){
		s.img[0] += x;
		s.img[1] += y;
	}
}
void AdaptiveSampler::get_samples(std::array<float, 2> *samples, int n_samples){
	LDSampler::sample2d(samples, n_samples, distrib(rng), distrib(rng));
	std::shuffle(samples, samples + n_samples, rng);
}
void AdaptiveSampler::get_samples(float *samples, int n_samples){
	LDSampler::sample1d(samples, n_samples, distrib(rng));
	std::shuffle(samples, samples + n_samples, rng);
}
int AdaptiveSampler::get_max_spp() const {
	return max_spp;
}
bool AdaptiveSampler::report_results(const std::vector<Sample> &samples,
	const std::vector<RayDifferential> &rays, const std::vector<Colorf> &colors)
{
	if (supersample_px == max_spp || !needs_supersampling(samples, rays, colors)){
		supersample_px = min_spp;
		++x;
		if (x == x_end){
			x = x_start;
			++y;
		}
		return true;
	}
	//Throw away these samples, we need to super sample this pixel
	supersample_px *= 2;
	return false;
}
std::vector<std::unique_ptr<Sampler>> AdaptiveSampler::get_subsamplers(int w, int h) const {
	int x_dim = x_end - x_start;
	int y_dim = y_end - y_start;
	std::vector<std::unique_ptr<Sampler>> samplers;
	if (w > x_dim || h > y_dim){
		std::cout << "WARNING: sampler cannot be partitioned to blocks bigger than itself\n";
		samplers.emplace_back(std::make_unique<AdaptiveSampler>(*this));
		return samplers;
	}
	//Compute the number of tiles to use in each dimension, we halve the number along x
	//and double the number along y until we hit an odd number of x tiles (cols) or
	//until the tiles divide the space about evenly
	int n_cols = x_dim / w;
	int n_rows = y_dim / h;
	x_dim /= n_cols;
	y_dim /= n_rows;
	//Check & warn if the space hasn't been split up evenly
	if (x_dim * n_cols != width() || y_dim * n_rows != height()){
		std::cout << "WARNING: sampler could not be partitioned equally into"
			<< " samplers of the desired dimensions " << w << " x " << h << std::endl;
	}
	for (int j = 0; j < n_rows; ++j){
		for (int i = 0; i < n_cols; ++i){
			samplers.emplace_back(std::make_unique<AdaptiveSampler>(i * x_dim + x_start,
				(i + 1) * x_dim + x_start, j * y_dim + y_start,
				(j + 1) * y_dim + y_start, min_spp, max_spp));
		}
	}
	return samplers;
}
bool AdaptiveSampler::needs_supersampling(const std::vector<Sample>&,
	const std::vector<RayDifferential>&, const std::vector<Colorf> &colors)
{
	const static float max_contrast = 0.3f;
	float lum_avg = 0;
	for (const auto &c : colors){
		lum_avg += c.luminance();
	}
	lum_avg /= colors.size();
	for (const auto &c : colors){
		if (std::abs(c.luminance() - lum_avg) / lum_avg > max_contrast){
			return true;
		}
	}
	return false;
}

