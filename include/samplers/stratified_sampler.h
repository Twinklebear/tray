#ifndef STRATIFIED_SAMPLER_H
#define STRATIFIED_SAMPLER_H

#include <memory>
#include <array>
#include <vector>
#include "sampler.h"

/*
 * A stratified sampler, generates multipled jittered
 * samples per pixel in its sample region
 */
class StratifiedSampler : public Sampler {
	//Samples per pixel
	const int spp;

public:
	StratifiedSampler(int x_start, int x_end, int y_start, int y_end, int spp);
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

