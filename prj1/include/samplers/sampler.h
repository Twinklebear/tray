#ifndef SAMPLER_H
#define SAMPLER_H

#include <array>
#include <vector>

/*
 * A basic sampler that samples the centers of each
 * pixel. The sample space can also be subdivided
 * so that the work can be distributed across threads
 */
class Sampler {
	int x_start, x_end, y_start, y_end, x, y;

public:
	/*
	 * Create a sampler for some region
	 */
	Sampler(int x_start, int x_end, int y_start, int y_end);
	/*
	 * Get an {x, y} position to sample in the space being sampled
	 * has_samples should be called prior to using this function
	 * otherwise the sample positions will not be valid
	 */
	std::array<float, 2> get_sample();
	/*
	 * Returns true if we haven't exhausted the sample space for
	 * the sampler yet
	 */
	bool has_samples();
	/*
	 * Get subsamplers that divide the space to be sampled
	 * into count disjoint subsections
	 * Note: compile with C++11 for move ctors, otherwise returning
	 * a vector is slow
	 */
	std::vector<Sampler> get_subsamplers(int count);
};

#endif

