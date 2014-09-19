#ifndef GAUSSIAN_FILTER_H
#define GAUSSIAN_FILTER_H

#include "filter.h"

struct GaussianFilter : Filter {
	const float alpha, exp_x, exp_y;

	GaussianFilter(float w, float h, float alpha);
	float weight(float x, float y) const override;

private:
	/*
	 * Compute the 1d Gaussian value using
	 * std::exp(-alpha * x * x) - expv
	 * where expv should be a pre-computed value
	 * for std::exp(-alpha * w * w)
	 */
	float gaussian_1d(float x, float expv) const;
};

#endif

