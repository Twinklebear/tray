#include <atomic>
#include "samplers/sampler.h"
#include "block_queue.h"

BlockQueue::BlockQueue(const Sampler &sampler, int blocks)
	: samplers(sampler.get_subsamplers(blocks)), sampler_idx(0)
{}
Sampler BlockQueue::get_block(){
	int n = samplers.size();
	if (sampler_idx.compare_exchange_strong(n, n, std::memory_order_acq_rel)){
		return Sampler{0, 0, 0, 0};
	}
	int s = sampler_idx.fetch_add(1, std::memory_order_acq_rel);
	//Potential race condition if we would have gotten the last block but some other
	//thread beat us from the cmp_exg to the fetch_add, so we need to double check
	if (s < samplers.size()){
		return samplers[s];
	}
	return Sampler{0, 0, 0, 0};
}

