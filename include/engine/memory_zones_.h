#pragma once

#include <unistd.h>
#include "memory_block.h"

// Minimum of allocation possible in a zone
#define NB_ALLOC 100

// Maximum alloc sizes for TINY and SMALL in bytes
#define TINY_MAX_ALLOC	512	 // 500bytes
#define SMALL_MAX_ALLOC 1024 // 1kilobytes

// Threshold to determine when a block goes to LARGE
#define LARGE_BLOCK_THRESHOLD (SMALL_MAX_ALLOC + 1)
// Similar to  MMAP_TRESHOLD in stdc malloc

typedef enum e_zonetype
{
	TINY,
	SMALL,
	LARGE,
} t_zonetype;

// Header for the memory zone
typedef struct s_zone_header
{
	t_zonetype				zone_type;
	size_t					total_size;
	size_t					space_used;
	// Top of the stack, next available block
	t_block_header			*top;
	struct s_zone_header	*next;
} t_zone_header;

// Singleton containing information like the
// starting memoryzone node or thread context
// Used for the global
typedef struct s_malloc_singleton
{
	int			   is_init;
	size_t		   page_size;
	t_zone_header *start;

} t_malloc_singleton;

// Find the correcto zone for the memory asked,
// and return correct block at the top of the
// memory zone
// @return: the adress which is guaranteed to have
// at least mem_asked of memory
// else NULL
void *allocate_memory(size_t mem_asked);
