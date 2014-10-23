#ifndef LD_SAMPLER_H
#define LD_SAMPLER_H

#include <random>
#include <memory>
#include <array>
#include <vector>
#include "sampler.h"

/*
 * A low discrepancy sampler based on (0, 2) sequences
 * as described in PBR
 */
class LDSampler : public Sampler {
	const int spp;
	std::mt19937_64 rng;
	std::uniform_int_distribution<uint32_t> distrib;

public:
	LDSampler(int x_start, int x_end, int y_start, int y_end, int spp);
	/*
	 * Get some {x, y} positions to sample in the space being sampled
	 * If the sampler has finished sampling samples will be empty
	 */
	void get_samples(std::vector<std::array<float, 2>> &samples) override;
	/*
	 * Get subsamplers that divide the space to be sampled
	 * into count disjoint subsections where each samples a w x h
	 * section of the original sampler
	 */
	std::vector<std::unique_ptr<Sampler>> get_subsamplers(int w, int h) const override;
	/*
	 * Generate a 2d pattern of low discrepancy samples and return them
	 * sample positions will be normalized between [0, 1)
	 */
	static void sample2d(std::vector<std::array<float, 2>> &samples, uint32_t x, uint32_t y);
	/*
	 * Generate a sample from a scrambled (0, 2) sequence
	 */
	static void sample02(uint32_t n, const std::array<uint32_t, 2> &scramble,
		std::array<float, 2> &sample);
	/*
	 * Generate a Van der Corput low discrepancy sequence value
	 * and scramble as described by Kollig & Keller (2002)
	 * This method is specialized for base 2
	 */
	static float van_der_corput(uint32_t n, uint32_t scramble);
	/*
	 * Generate a Sobol low discrepancy sequence value
	 * and scramble as described by Kollig & Keller (2002)
	 * This method is specialized for base 2
	 */
	static float sobol2(uint32_t n, uint32_t scramble);
};

#endif

