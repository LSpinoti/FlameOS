#ifndef MEM_H
#define MEM_H
#include <types.h>

#define BITS_PER_ELEMENT 32
#define HEAPS 2
#define FREE_SPACES 0x200
#define ALLOCATIONS 0x200

void *const kernel_heap		= (void *) 0x1800000000;
void *const kernel_stack	= (void *) 0x2000000000;
void *const userspace_heap	= (void *) 0x100000;
void *const userspace_stack	= (void *) 0x1000000000;

typedef struct {
	void *addr;
	size_t size;
} mem_t;

mem_t freespace[HEAPS][FREE_SPACES];
mem_t allocations[HEAPS][ALLOCATIONS];

#endif
