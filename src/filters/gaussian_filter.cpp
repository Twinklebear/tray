#include <cmath>
#include <algorithm>
#include "filters/gaussian_filter.h"

GaussianFilter::GaussianFilter(float w, float h, float alpha)
	: Filter(w, h), alpha(alpha), exp_x(std::exp(-alpha * w * w)),
	exp_y(std::exp(-alpha * h * h))
{}
float GaussianFilter::weight(float x, float y) const {
	return gaussian_1d(x, exp_x) * gaussian_1d(y, exp_y);
}
float GaussianFilter::gaussian_1d(float x, float expv) const {
	return std::max(0.f, std::exp(-alpha * x * x) - expv);
}

