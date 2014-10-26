#ifndef STRATIFIED_SAMPLER_H
#define STRATIFIED_SAMPLER_H

#include <random>
#include <memory>
#include <array>
#include <vector>
#include "sampler.h"

/*
 * A stratified sampler, generates multipled jittered
 * samples per pixel in its sample region
 */
class StratifiedSampler : public Sampler {
	const int spp;
	std::minstd_rand rng;
	std::uniform_real_distribution<float> distrib;

public:
	StratifiedSampler(int x_start, int x_end, int y_start, int y_end, int spp);
	/*
	 * Get some {x, y} positions to sample in the space being sampled
	 * If the sampler has finished sampling samples will be empty
	 */
	void get_samples(std::vector<Sample> &samples) override;
	/*
	 * Get the max number of samples this sampler will take per pixel
	 */
	int get_max_spp() const override;
	/*
	 * Get subsamplers that divide the space to be sampled
	 * into count disjoint subsections where each samples a w x h
	 * section of the original sampler
	 */
	std::vector<std::unique_ptr<Sampler>> get_subsamplers(int w, int h) const override;

private:
	/*
	 * Generate a 2d pattern of stratified samples and return them
	 * sample positions will be normalized between [0, 1)
	 */
	void sample2d(std::vector<std::array<float, 2>> &samples);
};

#endif

