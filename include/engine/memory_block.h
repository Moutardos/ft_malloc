#include "unistd.h"

// Block doesn't know the existence of zones,
// the pointer is void* and it's expected that the library will give the correct
// block_size for the zone

// Header for the memory block
typedef struct s_block_header
{
	size_t				   size;
	// Easier than is_free, since struct init at 0
	int					   is_not_free;
	void				  *zone_header;
	struct s_block_header *prev;
	struct s_block_header *next;
} t_block_header;

// Set the block as used
// The zone take care of the logic
// @return: pointer to the next free block
//			NULL if error
t_block_header *set_block(t_block_header *block, size_t size);

// Liberate the block, it is now the next one
// that's going to be used
// @return: pointer to itself
//			NULL if error
t_block_header *set_block_free(t_block_header *block);
