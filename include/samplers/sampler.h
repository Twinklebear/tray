#ifndef SAMPLER_H
#define SAMPLER_H

#include <array>
#include <random>
#include <vector>
#include <memory>
#include "linalg/ray.h"
#include "film/color.h"

/*
 * Sample positions on the image and on the lens generated by the sampler
 * img samples will be in the range the sampler covers while lens and time samples
 * are from [0, 1) and should be scaled into the desired range
 */
struct Sample {
	std::array<float, 2> img, lens;
	float time;
};

/*
 * A basic sampler that samples the centers of each
 * pixel. The sample space can also be subdivided
 * so that the work can be distributed across threads
 */
class Sampler {
protected:
	int x, y;
	std::minstd_rand rng;
	std::uniform_real_distribution<float> float_distrib;

public:
	const int x_start, x_end, y_start, y_end;
	/*
	 * Create a sampler for some region
	 */
	Sampler(int x_start, int x_end, int y_start, int y_end);
	/*
	 * Get some 5D samples to sample the image plane, lens and time dimensions
	 * If the sampler has finished sampling samples will be empty
	 */
	virtual void get_samples(std::vector<Sample> &samples) = 0;
	/*
	 * Get a set of 2D samples in range [0, 1)
	 * samples should already be allocated to contain the number of samples desired
	 */
	virtual void get_samples(std::vector<std::array<float, 2>> &samples) = 0;
	/*
	 * Get a set of 1D samples in range [0, 1)
	 * samples should already be allocated to contain the number of samples desired
	 */
	virtual void get_samples(std::vector<float> &samples) = 0;
	/*
	 * Get the max number of samples this sampler will take per pixel
	 */
	virtual int get_max_spp() const = 0;
	/*
	 * Report the results we got from sampling the scene using the samples
	 * provided by the sampler. The results should be discarded and a new
	 * set of samples taken if false is returned
	 * The default implementation simply returns true
	 */
	virtual bool report_results(const std::vector<Sample> &samples,
		const std::vector<RayDifferential> &rays, const std::vector<Colorf> &colors);
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

