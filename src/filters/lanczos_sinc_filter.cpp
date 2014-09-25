#include <cmath>
#include "linalg/util.h"
#include "filters/lanczos_sinc_filter.h"

LanczosSincFilter::LanczosSincFilter(float w, float h, float a)
	: Filter(w, h), a(a)
{}
float LanczosSincFilter::weight(float x, float y) const {
	return lanczos_sinc1d(x * inv_w) * lanczos_sinc1d(y * inv_h);
}
float LanczosSincFilter::lanczos_sinc1d(float x) const {
	float abs_x = std::abs(x);
	if (abs_x <= 1e-5){
		return 1;
	}
	if (abs_x >= a){
		return 0;
	}
	abs_x *= PI;
	return a * std::sin(abs_x) * std::sin(abs_x / a) / std::pow(abs_x, 2.f);
}

