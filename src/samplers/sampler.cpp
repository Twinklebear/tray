#include <iostream>
#include <array>
#include <vector>
#include "samplers/sampler.h"

Sampler::Sampler(int x_start, int x_end, int y_start, int y_end)
	: x(x_start), y(y_start), x_start(x_start), x_end(x_end),
	y_start(y_start), y_end(y_end)
{}
bool Sampler::has_samples(){
	return y != y_end;
}
int Sampler::width() const {
	return x_end - x_start;
}
int Sampler::height() const {
	return y_end - y_start;
}

