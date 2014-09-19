#ifndef LANCZOS_SINC_FILTER
#define LANCZOS_SINC_FILTER

#include "filter.h"

struct LanczosSincFilter : Filter {
	const float a;

	LanczosSincFilter(float w, float h, float a);
	float weight(float x, float y) const override;

private:
	float lanczos_sinc1d(float x) const;
};

#endif

