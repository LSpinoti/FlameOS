#include <types.h>
#include <macros.h>
#include <memory.h>
#include <string.h>
#include <globals.h>
#include "memory.h"
#include <syscall.h>

/*
	Allocate virtual memory from a heap
*/
void *malloc_heap(size_t size, int heap) {
	/* Error checking */
	if (heap >= HEAPS || size == 0)
		return 0;

	/* Look through freespace for large enough space */
	int space;
	for (space = 0; space < FREE_SPACES; ++space)
		if (freespace[heap][space].size >= size)
			goto exit1;
	return 0;
exit1:;

	/* Save address (this address will be returned) */
	void *const addr = freespace[heap][space].addr;

	/* Shrink the freespace size */
	freespace[heap][space].addr += size;
	freespace[heap][space].size -= size;

	/* Mark allocation in table */
	int alloc;
	for (alloc = 0; alloc < ALLOCATIONS; ++alloc)
		if (allocations[heap][alloc].size == 0)
			goto exit2;
	return 0;
	exit2:

	allocations[heap][alloc].addr = addr;
	allocations[heap][alloc].size = size;

	/* Map pages to that part if no pages are mapped there already */
	size_t pages = CEILING((size_t)(addr + size), PAGE_SIZE) - (size_t)(addr) / PAGE_SIZE;
	for (size_t i = 0; i < pages; ++i) {
		if (!find_physaddr(addr + i * PAGE_SIZE))
			map_page(phys_page_alloc(), addr + i * PAGE_SIZE);
	}
	return addr;
}

/*
	Allocate memory from the kernel heap
*/
void *malloc(size_t size) {
	return malloc_heap(size, 0);
}

/*
	Frees memory allocated from malloc
*/
void free(void *addr) {
	/* Heap/bounds checker */
	int heap;
	if (addr >= kernel_heap && addr < kernel_stack)
		heap = 0;
	else if (addr >= userspace_heap && addr < userspace_stack)
		heap = 1;
	else
		return;

	/* Find the allocation in the allocs table to find the size */
	int alloc;
	size_t size;
	for (alloc = 0; alloc < ALLOCATIONS; ++alloc)
		if (addr == allocations[heap][alloc].addr)
			goto exit1;
	return;
	exit1:
	size = allocations[heap][alloc].size;
	allocations[heap][alloc].addr = 0;
	allocations[heap][alloc].size = 0;
	bool returned = FALSE;
	for (int i = 0; i < FREE_SPACES; ++i) {
		/* Memory is behind freespace */
		if (addr + size == freespace[heap][i].addr) {
			freespace[heap][i].addr -= size;
			freespace[heap][i].size += size;
			returned = TRUE;
			break;
		}
		/* Memory is after freespace */
		if (addr == freespace[heap][i].addr + freespace[heap][i].size) {
			freespace[heap][i].size += size;
			returned = TRUE;
			break;
		}
	}
	if (!returned) {
		for (int i = 0; i < FREE_SPACES; ++i) {
			if (freespace[heap][i].size == 0) {
				freespace[heap][i].addr = addr;
				freespace[heap][i].size = size;
				goto exit2;
			}
		}
		return;
	}
	exit2:;

	/* Free pages (if there is no other memory in the page) */
	size_t pages = CEILING((size_t) addr + size, PAGE_SIZE) - (size_t) addr / PAGE_SIZE;
	bool sused = FALSE, eused = FALSE;
	for (int i = 0; i < ALLOCATIONS; ++i) {
		if (((size_t) addr / PAGE_SIZE) * PAGE_SIZE ==
			(((size_t) allocations[heap][i].addr + allocations[heap][i].size - 1) / PAGE_SIZE) * PAGE_SIZE)
			sused = TRUE;
		if ((((size_t) addr + size - 1) / PAGE_SIZE) * PAGE_SIZE ==
			((size_t) allocations[heap][i].addr / PAGE_SIZE) * PAGE_SIZE)
			eused = TRUE;
	}
	if (!sused) phys_page_free(find_physaddr(addr));
	if (!eused) phys_page_free(find_physaddr(addr + size - 1));
	for (int i = 1; i < pages - 1; ++i)
		phys_page_free(find_physaddr(addr + i * PAGE_SIZE));
}

/*
	Sets a page as used in the physical memory bitmap
	Returns the physical address of that page
*/
void *phys_page_alloc() {
	u64 i, j;
	u64 h = 0;
	u8 bit = 0;
	void *addr;
	u64 pages;

	for (i = 0; i < fm_descs; ++i) {
		addr  = freemap[i].addr;
		pages = freemap[i].blocks;
		for (j = 0; j < pages; ++j) {
			if (pmem_bmap[h] & 1 << bit) {
				addr += PAGE_SIZE;
				++bit;
				if (bit >= BITS_PER_ELEMENT) {
					bit = 0;
					++h;
				}
			}
			else {
				pmem_bmap[h] |= 1 << bit;
				return addr;
			}
		}
	}

	return 0;
}

/*
	Unsets the bit for a specific page in the physical memory bitmap
*/
void phys_page_free(void *addr) {
	if ((u64) addr % PAGE_SIZE) return;
	u64 i, j;
	u64 h = 0;
	u8 bit = 0;
	void *ptr;
	u64 pages;

	for (i = 0; i < mm_descs; ++i) {
		ptr   = freemap[i].addr;
		pages = freemap[i].blocks;
		for (j = 0; j < pages; ++j) {
			if (ptr == addr) {
				pmem_bmap[h] &= ~(1 << bit);
				return;
			}
			++bit;
			if (bit >= BITS_PER_ELEMENT) {
				bit = 0;
				++h;
			}
			ptr += PAGE_SIZE;
		}
	}
}

void *get_heap_pages(size_t pages, int heap) {

}

/* Gets *pages* number of physical pages and maps them to *virt* */
void alloc_and_map(const void *virt, size_t pages) {
	void *page;
	while (pages) {
		page = phys_page_alloc();
		map_page(page, (void *) virt);
		virt += PAGE_SIZE;
		--pages;
	}
}

/*
	Combines contiguous free spaces into one entry
*/
void refresh_freespace() {
	for (int i = 0; i < HEAPS; ++i) {
		for (int j = 0; j < FREE_SPACES; ++j) {
			void *end_addr = freespace[i][j].addr + freespace[i][j].size;
			for (int k = 0; k < FREE_SPACES; ++k) {
				if (freespace[i][k].addr == end_addr) {
					/* Combine the sizes */
					freespace[i][j].size += freespace[i][k].size;

					/* Delete old entry */
					freespace[i][k].addr = 0;
					freespace[i][k].size = 0;
				}
			}
		}
	}
}

void free_pages(const void *virt, size_t pages) {
	unsigned int pml4_idx;
	unsigned int pdpt_idx;
	unsigned int pd_idx;
	unsigned int pt_idx;

	while (pages) {
		pml4_idx =	((u64) virt >> 39) & 0x1FF;
		pdpt_idx =	((u64) virt >> 30) & 0x1FF;
		pd_idx =	((u64) virt >> 21) & 0x1FF;
		pt_idx =	((u64) virt >> 12) & 0x1FF;

		struct table *pdpt = ((void *) pml4.entry[pml4_idx]);
		struct table *pd = ((void *) pdpt->entry[pdpt_idx]);
		struct table *pt = ((void *) pd->entry[pd_idx]);

		pt->entry[pt_idx] &= ~(1 << 0);

		virt += PAGE_SIZE;
		--pages;
	}
}

void init_malloc() {
	freespace[0][0].addr = kernel_heap;
	freespace[0][0].size = kernel_stack - kernel_heap;
	freespace[1][0].addr = userspace_heap;
	freespace[1][0].size = userspace_stack - userspace_heap;
}

void init_paging();

void init_memory() {
	init_paging();
	init_malloc();
}
