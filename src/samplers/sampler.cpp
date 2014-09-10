#include <iostream>
#include <array>
#include <vector>
#include "samplers/sampler.h"

Sampler::Sampler(int x_start, int x_end, int y_start, int y_end)
	: x_start(x_start), x_end(x_end), y_start(y_start), y_end(y_end),
	x(x_start), y(y_start)
{}
std::array<float, 2> Sampler::get_sample(){
	std::array<float, 2> sample{x + 0.5f, y + 0.5f};
	//Advance to next sample position
	++x;
	if (x == x_end){
		x = x_start;
		++y;
	}
	return sample;
}
bool Sampler::has_samples(){
	return y != y_end;
}
int Sampler::width() const {
	return x_end - x_start;
}
int Sampler::height() const {
	return y_end - y_start;
}
std::vector<Sampler> Sampler::get_subsamplers(int w, int h) const {
	int x_dim = x_end - x_start;
	int y_dim = y_end - y_start;
	if (w > x_dim || h > y_dim){
		std::cout << "WARNING: sampler cannot be partitioned to blocks bigger than itself\n";
		return std::vector<Sampler>{*this};
	}
	//Compute the number of tiles to use in each dimension, we halve the number along x
	//and double the number along y until we hit an odd number of x tiles (cols) or
	//until the tiles divide the space about evenly
	int n_cols = x_dim / w;
	int n_rows = y_dim / h;
	
	std::vector<Sampler> samplers;
	x_dim /= n_cols;
	y_dim /= n_rows;
	//Check & warn if the space hasn't been split up evenly
	if (x_dim * n_cols != width() || y_dim * n_rows != height()){
		std::cout << "WARNING: sampler could not be partitioned equally into"
			<< " samplers of the desired dimensions " << w << " x " << h << std::endl;
	}
	for (int j = 0; j < n_rows; ++j){
		for (int i = 0; i < n_cols; ++i){
			samplers.emplace_back(i * x_dim + x_start, (i + 1) * x_dim + x_start,
				j * y_dim + y_start, (j + 1) * y_dim + y_start);
		}
	}
	return samplers;
}

