#ifndef UNIFORM_SAMPLER_H
#define UNIFORM_SAMPLER_H

#include <array>
#include <vector>
#include <memory>
#include "sampler.h"

/*
 * A very simple sampler, takes one sample per pixel at each
 * pixel's center in its sample region
 */
class UniformSampler : public Sampler {
public:
	/*
	 * Create a sampler for some region
	 */
	UniformSampler(int x_start, int x_end, int y_start, int y_end);
	/*
	 * Get an {x, y} position to sample in the space being sampled
	 * has_samples should be called prior to using this function
	 * otherwise the sample positions will not be valid
	 */
	std::array<float, 2> get_sample() override;
	/*
	 * Get subsamplers that divide the space to be sampled
	 * into count disjoint subsections where each samples a w x h
	 * section of the original sampler
	 */
	std::vector<std::unique_ptr<Sampler>> get_subsamplers(int w, int h) const override;
};

#endif

