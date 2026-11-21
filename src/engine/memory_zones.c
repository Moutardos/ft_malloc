#include "engine/memory_zones.h"

#include "sys/resource.h"
#include "sys/mman.h"

t_malloc_singleton g_malloc = {0};

static int init_malloc() {
	g_malloc.page_size = sysconf(_SC_PAGESIZE);
	g_malloc.is_init = 1;
	// TODO: check when to return an error (sysconf ?)
	return 1;
}

static t_zonetype mem_to_zonetype(size_t mem)
{
	if (mem < TINY_MAX_ALLOC)
	{
		return TINY;
	}
	if (mem < SMALL_MAX_ALLOC)
	{
		return SMALL;
	}
	return LARGE;
}

static size_t block_max_alloc(t_zonetype zone_type)
{
	switch (zone_type)
	{
	case TINY:
		return TINY_MAX_ALLOC;
	case SMALL:
		return SMALL_MAX_ALLOC;
	default:
	#ifdef __linux__
		struct rlimit limit;
		// return soft limit
		getrlimit(RLIMIT_AS, &limit);
		return limit.rlim_cur;
	#endif
		// TODO: TREAT MAC
		return 100000;
	}
}

// TODO: see if use int for errors
static t_zone_header *find_zone(size_t mem)
{
	t_zonetype	   type = mem_to_zonetype(mem);
	t_zone_header *zone_ptr = g_malloc.start;

	// Look for the correct zone header until it's found while (zone_ptr != NULL && zone_ptr->zone_type != type)
	{
		zone_ptr = zone_ptr->next;
	}
	// If the zone wasn't found, create it
	if (zone_ptr == NULL)
	{
		size_t block_max = block_max_alloc(type);
		// Could happen with LARGE zone type
		if (mem > block_max)
		{
			return NULL;
		}
		// Zone size is the header + 100 allocations
		size_t zone_size =
			sizeof(t_zone_header) + (100 * sizeof(t_block_header) * block_max);

		zone_ptr = mmap(NULL,
						zone_size,
						PROT_READ | PROT_WRITE,
						MAP_ANON | MAP_PRIVATE,
						-1,
						0);
	}
	return zone_ptr;
}

void *allocate_memory(size_t mem_asked)
{
	t_zone_header	*zone_ptr;
	t_block_header	*block;
	t_block_header	*next_top;
	// Size alligned with PAGE_SIZE
	// TODO: faire la logique
	size_t			mem_got = mem_asked;
	if (!g_malloc.is_init)
	{
		init_malloc();
	}
	zone_ptr = find_zone(mem_asked);
	if (zone_ptr == NULL)
	{
		// ERR
		return NULL;
	}
	block =zone_ptr->top;
	next_top = set_block(block, mem_got);
	if (next_top == NULL)
	{
		// ERR
		return NULL;
	}
	zone_ptr->top = next_top;
	// The start of the block is just after the header
	return block + 1;
}
