#include <chrono>
#include <algorithm>
#include <random>
#include <iostream>
#include <memory>
#include <array>
#include <vector>
#include "samplers/stratified_sampler.h"

StratifiedSampler::StratifiedSampler(int x_start, int x_end, int y_start, int y_end, int spp)
	: Sampler(x_start, x_end, y_start, y_end), spp(spp),
	rng(std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()).count())
{}
void StratifiedSampler::get_samples(std::vector<Sample> &samples){
	samples.clear();
	if (!has_samples()){
		return;
	}
	samples.resize(spp * spp);
	std::vector<std::array<float, 2>> pos(spp * spp), lens(spp * spp);
	std::vector<float> time(spp * spp);
	//Get a set of random samples in the range [0, 1) and scale them into pixel coords
	sample2d(pos);
	sample2d(lens);
	sample1d(time);
	std::shuffle(pos.begin(), pos.end(), rng);
	std::shuffle(lens.begin(), lens.end(), rng);
	std::shuffle(time.begin(), time.end(), rng);
	auto p = pos.begin();
	auto l = lens.begin();
	auto t = time.begin();
	auto s = samples.begin();
	for (; s != samples.end(); ++p, ++l, ++t, ++s){
		*s = Sample{*p, *l, 0};
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
int StratifiedSampler::get_max_spp() const {
	return spp;
}
std::vector<std::unique_ptr<Sampler>> StratifiedSampler::get_subsamplers(int w, int h) const {
	int x_dim = x_end - x_start;
	int y_dim = y_end - y_start;
	std::vector<std::unique_ptr<Sampler>> samplers;
	if (w > x_dim || h > y_dim){
		std::cout << "WARNING: sampler cannot be partitioned to blocks bigger than itself\n";
		samplers.emplace_back(std::make_unique<StratifiedSampler>(*this));
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
			samplers.emplace_back(std::make_unique<StratifiedSampler>(i * x_dim + x_start,
				(i + 1) * x_dim + x_start, j * y_dim + y_start,
				(j + 1) * y_dim + y_start, spp));
		}
	}
	return samplers;
}
void StratifiedSampler::sample1d(std::vector<float> &samples){
	float ds = 1.f / spp;
	for (int i = 0; i < samples.size(); ++i){
		samples[i] = (i + distrib(rng)) * ds;
	}
}
void StratifiedSampler::sample2d(std::vector<std::array<float, 2>> &samples){
	float ds = 1.f / spp;
	for (int i = 0; i < samples.size(); ++i){
		int x = i % spp;
		int y = i / spp;
		samples[i][0] = (x + distrib(rng)) * ds;
		samples[i][1] = (y + distrib(rng)) * ds;
	}
}

