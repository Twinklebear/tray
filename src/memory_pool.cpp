#include <iostream>
#include <cstring>
#include <algorithm>
#include "memory_pool.h"

MemoryPool::Block::Block(uint32_t size, char *block) : size(size), block(block){}

MemoryPool::MemoryPool(uint32_t block_size) : cur_block_pos(0), block_size(block_size),
	cur_block(block_size, new char[block_size])
{}
MemoryPool::~MemoryPool(){
	delete[] cur_block.block;
	for (auto &b : used){
		delete[] b.block;
	}
	for (auto &b : available){
		delete[] b.block;
	}
}
void MemoryPool::free_blocks(){
	while (!used.empty()){
#ifdef DEBUG
		std::memset(used.back().block, 255, used.back().size);
#endif
		available.push_back(used.back());
		used.pop_back();
	}
}
void* MemoryPool::alloc(uint32_t size){
	//Round size to minimum machine alignment
	size = (size + 15) & ~15;
	//If we need a new block to store this allocation
	if (cur_block_pos + size > block_size){
		used.push_back(cur_block);
		//If we've got an available block that can fit use that, otherwise allocate a new one
		auto block = std::find_if(available.begin(), available.end(),
			[size](const auto &b){
				return size <= b.size;
			});
		if (block != available.end()){
			cur_block = *block;
			available.erase(block);
		}
		else {
			uint32_t sz = std::max(size, block_size);
			cur_block = Block{sz, new char[sz]};
		}
		cur_block_pos = 0;
	}
	void *mem = cur_block.block + cur_block_pos;
	cur_block_pos += size;
	return mem;
}

