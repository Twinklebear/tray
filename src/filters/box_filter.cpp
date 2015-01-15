#include "filters/box_filter.h"

BoxFilter::BoxFilter(float w, float h) : Filter(w, h){}
float BoxFilter::weight(float, float) const {
	return 1;
}

