#ifndef SAMPLER_H
#define SAMPLER_H

#include <array>
#include <vector>
#include <memory>

/*
 * A basic sampler that samples the centers of each
 * pixel. The sample space can also be subdivided
 * so that the work can be distributed across threads
 */
class Sampler {
protected:
	int x, y;

public:
	const int x_start, x_end, y_start, y_end;
	/*
	 * Create a sampler for some region
	 */
	Sampler(int x_start, int x_end, int y_start, int y_end);
	/*
	 * Get some {x, y} positions to sample in the space being sampled
	 * If the sampler has finished sampling samples will be empty
	 */
	virtual void get_samples(std::vector<std::array<float, 2>> &samples) = 0;
	/*
	 * Returns true if we haven't exhausted the sample space for
	 * the sampler yet
	 */
	bool has_samples();
	/*
	 * Get the width and height of the sampler
	 */
	int width() const;
	int height() const;
	/*
	 * Get subsamplers that divide the space to be sampled
	 * into count disjoint subsections where each samples a w x h
	 * section of the original sampler
	 */
	virtual std::vector<std::unique_ptr<Sampler>> get_subsamplers(int w, int h) const = 0;
};

#endif

