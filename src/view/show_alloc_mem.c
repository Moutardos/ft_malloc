#include "libft.h"
#include "view/show_alloc_mem.h"
#include "engine/memory_zones.h"

// OUTPUT:
// TINY : 0xA0000
static void print_zone_header(const t_zone_header *zone_header)
{
	ft_printf("%s : %p\n", memory_zone_str(zone_header->zone_type), zone_header);
}

// OUTPUT:
// 0xA0020 - 0xA004A : 42 bytes
// 0xA006A - 0xA00BE : 84 bytes
static void print_block(const t_block_header *block_header)
{
	ft_printf("%X - %X : %d bytes\n",
			  block_header,
			  block_header + block_header->size,
			  block_header->size);
}
static void print_zone(const t_zone_header *zone)
{
	t_block_header *block = zone->start;

	print_zone_header(zone);
	while (block && block->is_not_free)
	{
		print_block(block);
		block = block->next;
	}
}

void show_alloc_mem()
{
	const t_zone_header *zone = get_first_zone();
	size_t				 total_malloc = get_total_malloc();

	while (zone)
	{
		print_zone(zone);
		zone = zone->next;
	}
	printf("Total : %lu bytes\n", total_malloc);
}
