#ifndef TRIANGLE_FILTER_H
#define TRIANGLE_FILTER_H

#include "filter.h"

/*
 * A simple triangle filter
 */
struct TriangleFilter : Filter {
	TriangleFilter(float w, float h);
	float weight(float x, float y) const override;
};

#endif

