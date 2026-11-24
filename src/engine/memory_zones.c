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

static t_zone_header *allocate_zone(size_t nb_block, size_t block_size)
{
	size_t		   zone_size;
	t_zone_header *zone_ptr;
	zone_size = align_up(sizeof(t_zone_header) +
							 (nb_block * (sizeof(t_block_header) + block_size)),
						 g_malloc.page_size);

	// MAP PRIVATE -> not acessible by other process
	// MAP ANON -> set everything to 0
	zone_ptr = mmap(
		NULL, zone_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

	if (zone_ptr == MAP_FAILED)
	{
		return NULL;
	}
	zone_ptr->total_size = zone_size;
	return zone_ptr;
}

// Allocate the zone memory and fill the header
static t_zone_header *create_zone(t_zone_header **link_to, t_zonetype type)
{
	// The maximum a block can store
	size_t block_max = align_up(block_max_alloc(type), ALIGN_VALUE);
	size_t nb_block = 1; // The zone is either 1 big block (if LARGE), else 100
	if (type != LARGE)
	{
		nb_block = 100;
	}
	size_t zone_hdr_aligned = align_up(sizeof(t_zone_header), ALIGN_VALUE);
	t_zone_header *zone_ptr;

	zone_ptr = allocate_zone(nb_block, block_max);
	if (!zone_ptr)
	{
		// TODO: ERR
		return NULL;
	}
	zone_ptr->block_size = block_max;
	zone_ptr->zone_type = type;
	// TODO : keep start at begining (free)
	zone_ptr->start =
		(t_block_header *)((uint8_t *)zone_ptr + zone_hdr_aligned);
	zone_ptr->top = zone_ptr->start;
	zone_ptr->next = NULL;
	*link_to = zone_ptr;
	return zone_ptr;
}

// TODO: see if use int for errors
// TODO: Better organization, srp
static t_zone_header *find_zone(t_zonetype type)
{
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
		zone_ptr = create_zone(link_to, type);
	}
	return zone_ptr;
}

void *allocate_block(size_t mem_asked)
{
	t_zone_header  *zone_ptr;
	t_block_header *block;
	t_zonetype		type = mem_to_zonetype(mem_asked);
	if (!g_malloc.is_init)
	{
		init_malloc();
	}
	zone_ptr = find_zone(type);
	if (zone_ptr == NULL)
	{
		// ERR
		return NULL;
	}
	block = zone_ptr->top;

	// TODO: add a error  return to set_block,
	// curr return NULL if good
	zone_ptr->top = set_block(block, zone_ptr->block_size);
	g_malloc.total_malloc += zone_ptr->block_size;
	// The start of the block is just after the header
	return block + 1;
}

char *memory_zone_str(t_zonetype zone_type)
{
	static char *map_zone_type_str[3] = {"TINY", "SMALL", "LARGE"};
	return map_zone_type_str[zone_type];
}

const t_zone_header *get_first_zone()
{
	return g_malloc.start;
}

size_t get_total_malloc()
{
	return g_malloc.total_malloc;
}
