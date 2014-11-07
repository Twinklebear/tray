#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <vector>

/*
 * A memory pool that allocates memory in chunks of the specified block size.
 * Only supports freeing of the entire pool, not individual allocations and can not
 * be moved or copied (it's possible to implement but I don't think I need it in the project)
 */
class MemoryPool {
	struct Block {
		uint32_t size;
		char *block;

		Block(uint32_t size, char *block);
	};

	uint32_t cur_block_pos, block_size;
	Block cur_block;
	std::vector<Block> used, available;

public:
	/*
	 * Create the memory pool specifying the block size to do allocations in (default is 32k)
	 */
	MemoryPool(uint32_t block_size = 32768);
	MemoryPool(const MemoryPool&) = delete;
	MemoryPool& operator=(const MemoryPool&) = delete;
	MemoryPool(const MemoryPool&&) = delete;
	MemoryPool& operator=(const MemoryPool&&) = delete;
	~MemoryPool();
	/*
	 * Construct a type in the memory pool
	 */
	template<typename T, typename... Args>
	T* alloc(Args&&... args){
		return new(alloc(sizeof(T))) T{std::forward<Args>(args)...};
	}
	/*
	 * Clear all used blocks, making them available again
	 */
	void free_blocks();

private:
	/*
	 * Allocate some number of bytes to be used for constructing some object
	 */
	void* alloc(uint32_t size);
};

#endif

