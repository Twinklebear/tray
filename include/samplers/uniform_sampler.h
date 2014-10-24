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
	 * Get some {x, y} positions to sample in the space being sampled
	 * If the sampler has finished sampling samples will be empty
	 */
	void get_samples(std::vector<Sample> &samples) override;
	/*
	 * Get subsamplers that divide the space to be sampled
	 * into count disjoint subsections where each samples a w x h
	 * section of the original sampler
	 */
	std::vector<std::unique_ptr<Sampler>> get_subsamplers(int w, int h) const override;
};

#endif

