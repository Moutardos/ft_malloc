#include "libft.h"
#include "ft_malloc.h"
#include "view/show_alloc_mem.h"
#include "engine/memory_zones.h"

int main(int ac, char **av)
{
	show_alloc_mem();
	allocate_block(10);
	show_alloc_mem();
}
