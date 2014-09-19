#include <chrono>
#include <random>
#include <iostream>
#include <memory>
#include <array>
#include <vector>
#include "samplers/stratified_sampler.h"

StratifiedSampler::StratifiedSampler(int x_start, int x_end, int y_start, int y_end, int spp)
	: Sampler(x_start, x_end, y_start, y_end), spp(spp)
{}
void StratifiedSampler::get_samples(std::vector<std::array<float, 2>> &samples){
	std::mt19937 rng(std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()).count());
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
std::vector<float> StratifiedSampler::sample2d(int nx, int ny, std::mt19937 &rng){
	std::uniform_real_distribution<float> distrib;
	std::vector<float> samples(2 * nx * ny);
	std::cout << "samples size: " << samples.size() << std::endl;
	float dx = 1.f / nx;
	float dy = 1.f / ny;
	for (int y = 0; y < ny; ++y){
		for (int x = 0; x < nx; ++x){
			samples[y * 2 + x] = (x + distrib(rng)) * dx;
			samples[y * 2 + x + 1] = (y + distrib(rng)) * dy;
		}
	}
	return samples;
}

