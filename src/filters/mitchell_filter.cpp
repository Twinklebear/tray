#include <cmath>
#include "filters/mitchell_filter.h"

MitchellFilter::MitchellFilter(float w, float h, float b, float c)
	: Filter(w, h), b(b), c(c)
{}
float MitchellFilter::weight(float x, float y) const {
	return mitchell_1d(x * inv_w) * mitchell_1d(y * inv_h);
}
float MitchellFilter::mitchell_1d(float x) const {
	float abs_x = std::abs(2 * x);
	//Filter for |x| >= 2 -> 0
	if (abs_x >= 2){
		return 0;
	}
	float filter = 0;
	//Filter for 1 <= |x| < 2
	if (abs_x >= 1){
		filter = (-b - 6.f * c) * std::pow(abs_x, 3.f)
			+ (6.f * b + 30.f * c) * std::pow(abs_x, 2.f)
			+ (-12.f * b - 48.f * c) * abs_x + 8.f * b + 24.f * c;
	}
	//Filter for |x| < 1
	else {
		filter = (12.f - 9.f * b - 6.f * c) * std::pow(abs_x, 3.f)
			+ (-18.f + 12.f * b + 6.f * c) * std::pow(abs_x, 2.f)
			+ 6.f - 2.f * b;
	}
	return filter / 6.f;
}


