#ifndef MITCHELL_FILTER_H
#define MITCHELL_FILTER_H

#include "filter.h"

struct MitchellFilter : Filter {
	const float b, c;

	MitchellFilter(float w, float h, float b, float c);
	float weight(float x, float y) const override;

private:
	float mitchell_1d(float x) const;
};

#endif

