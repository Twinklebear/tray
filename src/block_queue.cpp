#include <chrono>
#include <atomic>
#include <iostream>
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
	: samplers(sampler.get_subsamplers(bwidth, bheight)), sampler_idx(0), total_time(0)
{
	//Sort the samplers in Morton order
	std::sort(samplers.begin(), samplers.end(),
		[](const std::unique_ptr<Sampler> &a, const std::unique_ptr<Sampler> &b){
			return morton2(a->x_start, a->y_start) < morton2(b->x_start, b->y_start);
		});
}
Sampler* BlockQueue::get_block(){
	//I doubt I'll ever run this on an image big enough to make overflowing back to the 1st sampler
	//a concern here, especially since threads exit after getting a null sampler
	unsigned int s = sampler_idx.fetch_add(1, std::memory_order_acq_rel);
	if (s >= samplers.size()){
		return nullptr;
	}
	//Only one thread will get the sampler with this number unless we're running
	//really fast, so should be ok here
	if (s % (samplers.size() / 10) == 0){
		std::cout << "Starting work on block " << s << " of " << samplers.size()
			<< " : ~" << 100.f * static_cast<float>(s) / samplers.size() << "% of pixels completed"
			<< std::endl;
		if (s == 0){
			prev = std::chrono::high_resolution_clock::now();
		}
		else {
			auto now = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev);
			auto avg_block = (total_time.count() + elapsed.count()) / s;
			std::cout << "Average block time: " << avg_block << "ms"
				<< "\nRender time so far: " << total_time.count() << "ms"
				<< "\nEstimated remaining time: " << avg_block * (samplers.size() - s) << "ms\n";
			total_time += elapsed;
			prev = now;
		}
	}
	return samplers[s].get();
}

