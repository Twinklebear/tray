#include <iostream>
#include <array>
#include <vector>
#include "samplers/sampler.h"

Sampler::Sampler(int x_start, int x_end, int y_start, int y_end)
	: x_start(x_start), x_end(x_end), y_start(y_start), y_end(y_end),
	x(x_start), y(y_start)
{}
bool Sampler::has_samples(){
	return y != y_end;
}
int Sampler::get_x_start() const {
	return x_start;
}
int Sampler::get_y_start() const {
	return y_start;
}
int Sampler::width() const {
	return x_end - x_start;
}
int Sampler::height() const {
	return y_end - y_start;
}

