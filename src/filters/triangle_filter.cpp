#include <algorithm>
#include "filters/triangle_filter.h"

TriangleFilter::TriangleFilter(float w, float h) : Filter(w, h){}
float TriangleFilter::weight(float x, float y) const {
	return std::max(0.f, w - std::abs(x)) * std::max(0.f, h - std::abs(y));
}

