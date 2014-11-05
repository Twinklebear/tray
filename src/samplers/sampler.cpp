#include <random>
#include <chrono>
#include <array>
#include <vector>
#include "samplers/sampler.h"

Sampler::Sampler(int x_start, int x_end, int y_start, int y_end)
	: x(x_start), y(y_start), rng(std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()).count()),
	 x_start(x_start), x_end(x_end), y_start(y_start), y_end(y_end)
{}
bool Sampler::report_results(const std::vector<Sample>&,
	const std::vector<RayDifferential>&, const std::vector<Colorf>&)
{
	return true;
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

