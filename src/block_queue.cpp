#include <atomic>
#include <vector>
#include <algorithm>
#include "samplers/sampler.h"
#include "block_queue.h"

BlockQueue::BlockQueue(const Sampler &sampler, int bwidth, int bheight)
	: samplers(sampler.get_subsamplers(bwidth, bheight)), sampler_idx(0)
{
	//I wonder if it'd be possible to do this in-place? Maybe more complicated
	//that I want to get into though
	int cols = sampler.width() / samplers.at(0).width();
	int rows = sampler.height() / samplers.at(0).height();
	int x_start = 0, x_end = cols - 1;
	int y_start = 0, y_end = rows - 1;
	std::vector<Sampler> spiral;
	while (true){
		for (int i = x_start; i <= x_end; ++i){
			spiral.push_back(samplers[y_start * cols + i]);
		}
		++y_start;

		if (spiral_done(x_start, x_end, y_start, y_end)){
			break;
		}
		for (int j = y_start; j <= y_end; ++j){
			spiral.push_back(samplers[j * cols + x_end]);
		}
		--x_end;

		if (spiral_done(x_start, x_end, y_start, y_end)){
			break;
		}
		for (int i = x_end; i >= x_start; --i){
			spiral.push_back(samplers[y_end * cols + i]);
		}
		--y_end;

		if (spiral_done(x_start, x_end, y_start, y_end)){
			break;
		}
		for (int j = y_end; j >= y_start; --j){
			spiral.push_back(samplers[j * cols + x_start]);
		}
		++x_start;

		if (spiral_done(x_start, x_end, y_start, y_end)){
			break;
		}
	}
	//The spiral starts at the top-left and goes in clockwise so to work out from
	//the center just reverse it
	samplers = std::move(spiral);
	std::reverse(samplers.begin(), samplers.end());
}
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
bool BlockQueue::spiral_done(int x_start, int x_end, int y_start, int y_end){
	return x_start > x_end || y_start > y_end;
}

