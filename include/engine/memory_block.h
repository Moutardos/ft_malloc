#include "unistd.h"

// Block doesn't know the existence of zones,
// the pointer is void* and it's expected that the library will give the correct
// block_size for the zone

// Header for the memory block
typedef struct s_block_header
{
	size_t				   block_size;
	int					   is_free;
	void				  *zone_header;
	struct s_block_header *next;
} t_block_header;

void *return_block(size_t mem_size);
