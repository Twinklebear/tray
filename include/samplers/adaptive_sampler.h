#ifndef ADAPTIVE_SAMPLER_H
#define ADAPTIVE_SAMPLER_H

#include <array>
#include <vector>
#include <memory>
#include <random>
#include "linalg/ray.h"
#include "film/color.h"
#include "sampler.h"

/*
 * A sampler that adapts the number of samples taken for each pixel
 * based on the contrast difference between the samples taken
 */
class AdaptiveSampler : public Sampler {
	const int min_spp, max_spp;
	//If the current pixel we're sampling needs super sampling
	bool supersample_px;
	std::uniform_int_distribution<uint32_t> distrib;

public:
	/*
	 * Initialize the adaptive sampler to sample some region taking at least min_spp samples
	 * per pixel and at most max_spp samples per pixel if a pixel in the region needs supersampling
	 */
	AdaptiveSampler(int x_start, int x_end, int y_start, int y_end, int min_spp, int max_spp);
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
	 * Report the results we got from sampling the scene using the samples
	 * provided by the sampler. The results should be discarded and a new
	 * set of samples taken if false is returned
	 * The default implementation simply returns true
	 */
	bool report_results(const std::vector<Sample> &samples,
		const std::vector<RayDifferential> &rays, const std::vector<Colorf> &colors) override;
	/*
	 * Get subsamplers that divide the space to be sampled
	 * into count disjoint subsections where each samples a w x h
	 * section of the original sampler
	 */
	std::vector<std::unique_ptr<Sampler>> get_subsamplers(int w, int h) const override;

private:
	/*
	 * Determine if the pixel we've sampled needs supersampling by inspecting the contrast
	 * between the samples. Returns true if more samples should be taken
	 */
	bool needs_supersampling(const std::vector<Sample> &samples,
		const std::vector<RayDifferential> &rays, const std::vector<Colorf> &colors);
};

#endif

