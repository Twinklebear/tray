#include "filters/box_filter.h"

BoxFilter::BoxFilter(float w, float h) : Filter(w, h){}
float BoxFilter::weight(float x, float y) const {
	return 1;
}

