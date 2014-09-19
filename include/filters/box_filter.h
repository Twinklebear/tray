#ifndef BOX_FILTER_H
#define BOX_FILTER_H

#include "filter.h"

/*
 * A simple box filter
 */
struct BoxFilter : Filter {
	BoxFilter(float w, float h);
	float weight(float x, float y) const override;
};

#endif

