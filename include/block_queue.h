#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <atomic>
#include "samplers/sampler.h"

/*
 * A queue that hands out blocks of pixels to be worked on
 * by threads in the form of samplers for the block of pixels
 */
class BlockQueue {
	std::vector<Sampler> samplers;
	//The index of the next sampler to be handed out
	std::atomic_int sampler_idx;

public:
	/*
	 * Create a queue of work blocks by subsampling the sampler
	 * into blocks subsamplers
	 */
	BlockQueue(const Sampler &sampler, int bwidth, int bheight);
	/*
	 * Return the next block to be worked on, returns
	 * a sampler without samples when all samplers have
	 * been taken
	 */
	Sampler get_block();

private:
	/*
	 * Check if the spiral iteration is finished
	 */
	static bool spiral_done(int x_start, int x_end, int y_start, int y_end);
};

#endif

