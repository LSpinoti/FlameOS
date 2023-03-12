#ifndef MEMORY_H
#define MEMORY_H
#include <types.h>

#define PAGE_SIZE 4096

typedef struct {
	void *addr;
	size_t blocks;
} memory_t;

typedef struct {
	u32 type;
	u32 pad;
	void *physaddr;
	void *virtaddr;
	u64 pages;
	u64 attr;
} memmap_t;

typedef struct {
	void *addr;
	size_t blocks;
} section;

void init_memory();
void map_page(void *phys, void *virt);
void *find_physaddr(void *virt);
void *phys_page_alloc();
void phys_page_free(void *addr);

#endif
