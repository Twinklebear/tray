#ifndef DISTRIBUTION1D_H
#define DISTRIBUTION1D_H

#include <vector>

/*
 * Represents a piecewise-constant 1D function's PDF and CDF, as described in PBR
 */
class Distribution1D {
	std::vector<float> function, cdf;
	float integral;

public:
	/*
	 * Create the sampling distribution for the passed function values
	 */
	Distribution1D(const std::vector<float> &function);
	Distribution1D();
	/*
	 * Sample one of the buckets of the function and return the bucket
	 * number sampled, u should be in the range [0, 1)
	 * optionally returns the pdf of sampling the bucket that was sampled
	 */
	int sample_discrete(float u, float *pdf_val = nullptr) const;
};

#endif

