#include <chrono>
#include <iostream>
#include <random>
#include <memory>
#include <array>
#include <vector>
#include "linalg/util.h"
#include "samplers/ld_sampler.h"

LDSampler::LDSampler(int x_start, int x_end, int y_start, int y_end, int sp)
	: Sampler(x_start, x_end, y_start, y_end), spp(round_up_pow2(sp))
{
	if (sp % 2 != 0){
		std::cout << "Warning: LDSampler requires power of 2 samples per pixel."
			<< " Rounded spp up to " << spp << std::endl;
	}
}
void LDSampler::get_samples(std::vector<Sample> &samples){
	samples.clear();
	if (!has_samples()){
		return;
	}
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
	++x;
	if (x == x_end){
		x = x_start;
		++y;
	}
}
void LDSampler::get_samples(std::array<float, 2> *samples, int n_samples){
	sample2d(samples, n_samples, distrib(rng), distrib(rng));
	std::shuffle(samples, samples + n_samples, rng);
}
void LDSampler::get_samples(float *samples, int n_samples){
	sample1d(samples, n_samples, distrib(rng));
	std::shuffle(samples, samples + n_samples, rng);
}
int LDSampler::get_max_spp() const {
	return spp;
}
std::vector<std::unique_ptr<Sampler>> LDSampler::get_subsamplers(int w, int h) const {
	int x_dim = x_end - x_start;
	int y_dim = y_end - y_start;
	std::vector<std::unique_ptr<Sampler>> samplers;
	if (w > x_dim || h > y_dim){
		std::cout << "WARNING: sampler cannot be partitioned to blocks bigger than itself\n";
		samplers.emplace_back(std::make_unique<LDSampler>(*this));
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
			samplers.emplace_back(std::make_unique<LDSampler>(i * x_dim + x_start,
				(i + 1) * x_dim + x_start, j * y_dim + y_start,
				(j + 1) * y_dim + y_start, spp));
		}
	}
	return samplers;
}
void LDSampler::sample1d(float *samples, int n_samples, uint32_t scramble, int offset){
	for (int i = 0; i < n_samples; ++i){
		samples[i] = van_der_corput(i + offset, scramble);
	}
}
void LDSampler::sample2d(std::array<float, 2> *samples, int n_samples, uint32_t x, uint32_t y, int offset){
	std::array<uint32_t, 2> scramble{x, y};
	for (int i = 0; i < n_samples; ++i){
		sample02(i + offset, scramble, samples[i]);
	}
}
void LDSampler::sample02(uint32_t n, const std::array<uint32_t, 2> &scramble,
	std::array<float, 2> &sample)
{
	sample[0] = van_der_corput(n, scramble[0]);
	sample[1] = sobol2(n, scramble[1]);
}
float LDSampler::van_der_corput(uint32_t n, uint32_t scramble){
	n = (n << 16) | (n >> 16);
	n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
	n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
	n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
	n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
	n ^= scramble;
	return ((n >> 8) & 0xffffff) / float{1 << 24};
}
float LDSampler::sobol2(uint32_t n, uint32_t scramble){
	for (uint32_t i = 1 << 31; n != 0; n >>= 1, i ^= i >> 1){
		if (n & 0x1){
			scramble ^= i;
		}
	}
	return ((scramble >> 8) & 0xffffff) / float{1 << 24};
}

