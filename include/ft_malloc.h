#pragma once

#include "stdio.h"

// Free the memory allocated in `ptr`
void    free(void *ptr);

// Allocated `size` bytes of memory
// Return the pointer at the start of the new allocated memory
// NULL if error is met
void    *malloc(size_t size);

// Change the size of allocation in `ptr` by `size`;
// If the original allocation doesn't have enough space, copies the old value into
// a new allocation and free the old one.
// Return the pointer to the new allocated memory or NULL if error is met
void    *realloc(void *ptr, size_t size);
