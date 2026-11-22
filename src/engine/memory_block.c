#include "engine/memory_block.h"

t_block_header *set_block_used(t_block_header *block, size_t mem_size) 
{
	if (block == NULL)
	{
		// ERROR: ? is null!
		return NULL;
	}
	if (block->is_not_free)
	{
		// ERROR: block is already used!
		return NULL;
	}

	block->size = mem_size;
	block->is_not_free = 1;
	return block->next;
}

// Zone should use this as the next block available
t_block_header *set_block_free(t_block_header *block)

{
	block->is_not_free = 0;
	return block;
}
