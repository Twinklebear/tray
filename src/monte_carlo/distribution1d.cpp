#include <algorithm>
#include "monte_carlo/distribution1d.h"

Distribution1D::Distribution1D(const std::vector<float> &fcn)
	: function(fcn), cdf(function.size() + 1), integral(0)
{
	//Compute the integral of the function
	for (size_t i = 1; i < cdf.size(); ++i){
		cdf[i] = cdf[i - 1] + function[i - 1] / function.size();
	}
	integral = cdf.back();
	//Normalize the CDF by the integral
	for (auto &c : cdf){
		c /= integral;
	}
}
Distribution1D::Distribution1D(){}
int Distribution1D::sample_discrete(float u) const {
	auto a = std::lower_bound(cdf.begin(), cdf.end(), u);
	return std::distance(cdf.begin(), a);
}

