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

public:
	StratifiedSampler(int x_start, int x_end, int y_start, int y_end, int spp, int seed);
	StratifiedSampler(int x_start, int x_end, int y_start, int y_end, int spp);
	/*
	 * Get some {x, y} positions to sample in the space being sampled
	 * If the sampler has finished sampling samples will be empty
	 */
	void get_samples(std::vector<Sample> &samples) override;
	/*
	 * Get a set of 2D samples in range [0, 1)
	 * Note that the stratified sampler ignores the offset value
	 */
	void get_samples(std::array<float, 2> *samples, int n_samples, int offset = 0) override;
	/*
	 * Get a set of 1D samples in range [0, 1)
	 * Note that the stratified sampler ignores the offset value
	 */
	void get_samples(float *samples, int n_samples, int offset = 0) override;
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
	/*
	 * Generate a 1d pattern of stratified samples and return them
	 * samples will be normalized between [0, 1)
	 */
	static void sample1d(float *samples, int n_samples, std::minstd_rand &rng);
	/*
	 * Generate a 2d pattern of stratified samples and return them
	 * sample positions will be normalized between [0, 1)
	 */
	static void sample2d(std::array<float, 2> *samples, int n_samples, std::minstd_rand &rng);
};

#endif

