#pragma once

#include <unistd.h>

// Minimum of allocation possible in a zone
#define NB_ALLOC 100

// Maximum alloc sizes for TINY and SMALL in bytes
#define TINY_MAX_ALLOC    1024       // 1Kilobytes
#define SMALL_MAX_ALLOC   16384      // 16kilobytes

// Threshold to determine when a block goes to LARGE
#define LARGE_BLOCK_THRESHOLD (SMALL_MAX_ALLOC + 1)
// Similar to  MMAP_TRESHOLD in stdc malloc

typedef enum e_zonetype { TINY, SMALL, LARGE } t_zonetype;

// Header for the memory zone
typedef struct s_memoryzone_header {
    t_zonetype zone_type;
    size_t total_size;
    size_t free_space;
    struct s_memoryzone_header* next;
} t_memoryzone_header;

// Header for individual allocation TINY or SMALL block
typedef struct s_block_header {
    size_t block_size;
    int is_free;
    t_memoryzone_header zone_header;
    struct s_block_header* next;
} t_block_header;

// Singleton containing information like the
// starting memoryzone node or thread context
// Used for the global
typedef struct s_malloc_singleton {
    t_memoryzone_header* start;
} t_malloc_singleton;

// Find a corresponding block or create a new zone of memory
// return the adress or null if something wrong happened
void* allocate_block(size_t block_size);
