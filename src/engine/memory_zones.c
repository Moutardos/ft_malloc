#include <sys/resource.h>
#include <sys/mman.h>
#include <stdint.h>

#include "engine/memory_zones.h"
#include "engine/memory.h"
t_malloc_singleton g_malloc = {0};

static int init_malloc()
{
	g_malloc.page_size = sysconf(_SC_PAGESIZE);
	if (g_malloc.page_size <= 0)
	{
		// WARNING LOG
		// Could happen
		g_malloc.page_size = DEFAULT_PAGE_SIZE;
	}
	g_malloc.is_init = 1;
	return 1;
}

static t_zonetype mem_to_zonetype(size_t mem)
{

	size_t max_safe = SIZE_MAX - (g_malloc.page_size * 2);
	if (mem < TINY_MAX_ALLOC)
	{
		return TINY;
	}
	if (mem < SMALL_MAX_ALLOC)
	{
		return SMALL;
	}
	struct rlimit limit;
	// return soft limit
	if (getrlimit(RLIMIT_AS, &limit))
	{
		// fail err
		return OVER;
	}
	if (mem < max_safe && mem < limit.rlim_cur)
	{
		return LARGE;
	}
	// ERR
	return OVER;
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
		// not used
		return 0;
	}
}

// Allocate the zone memory and fill the header
static t_zone_header *
allocate_zone(t_zone_header **link_to, t_zonetype type, size_t requested_size)
{
	// The maximum a block can alloc
	size_t block_max = align_up(block_max_alloc(type), ALIGN_VALUE);
	// Zone size is the header + 100 allocations
	// or the requested_size if the type is large
	size_t zone_size = 0;
	if (type == LARGE)
	{
		zone_size = align_up(sizeof(t_zone_header) +
								 (sizeof(t_block_header) + requested_size),
							 g_malloc.page_size);
	}
	else
	{
		zone_size = align_up(sizeof(t_zone_header) +
								 (100 * (sizeof(t_block_header) + block_max)),
							 g_malloc.page_size);
	}
	// MAP ANON -> set everything to 0
	// MAP PRIVATE -> not acessible by other process
	t_zone_header *zone_ptr = mmap(
		NULL, zone_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

	if (zone_ptr == MAP_FAILED)
	{
		return NULL;
	}
	size_t zone_hdr_aligned = align_up(sizeof(t_zone_header), ALIGN_VALUE);
	zone_ptr->next = NULL;
	zone_ptr->zone_type = type;
	zone_ptr->total_size = zone_size - zone_hdr_aligned;
	zone_ptr->top = (t_block_header *)((uint8_t *)zone_ptr + zone_hdr_aligned);
	*link_to = zone_ptr;
	return zone_ptr;
}

// TODO: see if use int for errors
// TODO: Better organization, srp
static t_zone_header *find_zone(size_t mem)
{
	t_zonetype type = mem_to_zonetype(mem);
	// link_to is the link from the previous node to the new zone if created
	t_zone_header **link_to = &g_malloc.start;
	t_zone_header  *zone_ptr = g_malloc.start;

	if (type == OVER)
	{
		// ERR mem is too big
		return NULL;
	}
	// Look for the correct zone header until it's found
	while (zone_ptr != NULL && (type == LARGE || zone_ptr->zone_type != type))
	{
		link_to = &zone_ptr->next;
		zone_ptr = zone_ptr->next;
	}
	// If the zone wasn't found, create it
	if (zone_ptr == NULL)
	{
		zone_ptr = allocate_zone(link_to, type, mem);
	}
	return zone_ptr;
}

void *allocate_block(size_t mem_asked)
{
	t_zone_header  *zone_ptr;
	t_block_header *block;
	t_block_header *next_top;

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
	block = zone_ptr->top;
	next_top = set_block(block, mem_asked);
	if (next_top == NULL)
	{
		// ERR
		return NULL;
	}
	zone_ptr->top = next_top;
	// The start of the block is just after the header
	return block + 1;
}

