#include "memory.h"
size_t align_up(size_t n, size_t align)
{
	// make sure that n is a higher multiple of align
	// Doesnt work if align is not a power of 2
	return ((n + (align - 1)) & ~(align - 1));
}
