#include <atomic>
#include <vector>
#include <algorithm>
#include "samplers/sampler.h"
#include "block_queue.h"

//Fabian Giesen's Morton code generation
//See: http://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/
static uint32_t part1_by1(uint32_t x){
	// x = ---- ---- ---- ---- fedc ba98 7654 3210
	x &= 0x0000ffff;
	// x = ---- ---- fedc ba98 ---- ---- 7654 3210
	x = (x ^ (x << 8)) & 0x00ff00ff;
	// x = ---- fedc ---- ba98 ---- 7654 ---- 3210
	x = (x ^ (x << 4)) & 0x0f0f0f0f;
	// x = --fe --dc --ba --98 --76 --54 --32 --10
	x = (x ^ (x << 2)) & 0x33333333;
	// x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
	x = (x ^ (x << 1)) & 0x55555555;
	return x;
}
static uint32_t morton2(uint32_t x, uint32_t y){
	return (part1_by1(y) << 1) + part1_by1(x);
}

BlockQueue::BlockQueue(const Sampler &sampler, int bwidth, int bheight)
	: samplers(sampler.get_subsamplers(bwidth, bheight)), sampler_idx(0)
{
	//Sort the samplers in Morton order
	std::sort(samplers.begin(), samplers.end(),
		[](const std::unique_ptr<Sampler> &a, const std::unique_ptr<Sampler> &b){
			return morton2(a->x_start, a->y_start) < morton2(b->x_start, b->y_start);
		});
}
Sampler* BlockQueue::get_block(){
	int n = samplers.size();
	if (sampler_idx.compare_exchange_strong(n, n, std::memory_order_acq_rel)){
		return nullptr;
	}
	int s = sampler_idx.fetch_add(1, std::memory_order_acq_rel);
	//Potential race condition if we would have gotten the last block but some other
	//thread beat us from the cmp_exg to the fetch_add, so we need to double check
	if (s < samplers.size()){
		return samplers[s].get();
	}
	return nullptr;
}

