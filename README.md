# FT_MALLOC

42 introduction to memory allocation,
create a lib that'll contain my version of malloc

## Instructions

library will be named libft_malloc_$HOSTTYPE with a symbolic  link name `libft_malloc.so`

> `$HOSTTYPE = 'uname -m'_'uname -s\'`  

one global variable is allowed to manage allocations / thread safety

### Allowed functions:
- mmap
- munmap
- sysconf(_SC_PAGESIZE)
- getrlimit
- libft functions
- libpthread (bonus)

## Mandatory

### Lib Functions
```c
// deallocate memory pointed by ptr
void    free(void *ptr);
// Allocated `size` bytes of memory
// Return the pointer to the start of the new allocated memory
// or NULL if error is met
void    *malloc(size_t size);
// Change the size of allocation in `ptr` by `size`;
// If the original allocation doesn't have enough space, copies the old value into
// a new allocation and free the old one.
// Return the pointer to the new allocated memory or NULL if error is met
void    *realloc(void *ptr, size_t size);
```

mmap and munmap system calls to request memory zones of the system
*Perfomance*: Limit the number of calls to mmap and munmap.

Preloaded memory zones has to be used. Their sizes are multiple of sysconf(_SC_PAGESIZE)
> Each zone must have at least 100 allocations

TINY - 1 to n bytes in N bytes zones
SMALL - from (n+1) to m bytes in M bytes zones
LARGE - from (m+1 bytes) and more. allocated outside of zone via the call to mmap.

### Visualizer
```c
void show_alloc_mem();
```

*OUTPUT*:
```
TINY : 0xA0000
0xA0020 - 0xA004A : 42 bytes
0xA006A - 0xA00BE : 84 bytes
SMALL : 0xAD000
0xAD020 - 0xADEAD : 3725 bytes
LARGE : 0xB0000
0xB0020 - 0xBBEEF : 48847 bytes
Total : 52698 bytes
```

## Bonus

- Multi thread
- Use env variable (see system-malloc)
- Defragment freed memory


## Learning

### HOW

Main idea: mmap give us a big zone of "memory" to play with,
we divided it inside by *block* of different size (tiny small large)
LARGE should be it's own thing. idk yet lol

### What is a page?
What they mean by pages (sys_conf) is the way memory is divided logicaly (opposed to physically) for the process.
Logical -> pages
Physical -> frames
To give the process enough space that's next to each other, even if it's not the case physically.

Block allocation should be always to the closest biggest multiple of  pagesize

### Header:
Header before block:
- can give the size
- if it's already free or not.
Header before zone (given by mmap):
- pointer to next zone
- type of zone (tiny/small/large)
o
