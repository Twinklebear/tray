#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include "samplers/uniform_sampler.h"

UniformSampler::UniformSampler(int x_start, int x_end, int y_start, int y_end)
	: Sampler(x_start, x_end, y_start, y_end)
{}
std::array<float, 2> UniformSampler::get_sample(){
	std::array<float, 2> sample{x + 0.5f, y + 0.5f};
	//Advance to next sample position
	++x;
	if (x == x_end){
		x = x_start;
		++y;
	}
	return sample;
}
std::vector<std::unique_ptr<Sampler>> UniformSampler::get_subsamplers(int w, int h) const {
	int x_dim = x_end - x_start;
	int y_dim = y_end - y_start;
	std::vector<std::unique_ptr<Sampler>> samplers;
	if (w > x_dim || h > y_dim){
		std::cout << "WARNING: sampler cannot be partitioned to blocks bigger than itself\n";
		samplers.emplace_back(std::make_unique<UniformSampler>(*this));
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
			samplers.emplace_back(std::make_unique<UniformSampler>(i * x_dim + x_start,
				(i + 1) * x_dim + x_start, j * y_dim + y_start,
				(j + 1) * y_dim + y_start));
		}
	}
	return samplers;
}

