#include <stdlib.h>
#include <macros.h>
#include <string.h>
#include <globals.h>
#include "paging.h"

void map_page(void *phys, void *virt) {
	if ((u64) phys % PAGE_SIZE || (u64) virt % PAGE_SIZE) return;

	unsigned int pml4_idx = ((u64) virt >> 39) & 0x1FF;
	unsigned int pdpt_idx = ((u64) virt >> 30) & 0x1FF;
	unsigned int pd_idx   = ((u64) virt >> 21) & 0x1FF;
	unsigned int pt_idx   = ((u64) virt >> 12) & 0x1FF;

	if (!(pml4.entry[pml4_idx] & FLAG_PRESENT)) {
		void* pdpt_page = phys_page_alloc();
		memset(pdpt_page, 0, PAGE_SIZE);
		pml4.entry[pml4_idx] = (u64) pdpt_page & ADDR_MASK | FLAGS;
		map_page(pdpt_page, pdpt_page);
	}

	struct table *pdpt = (void *) (pml4.entry[pml4_idx] & ADDR_MASK);
	if (!(pdpt->entry[pdpt_idx] & FLAG_PRESENT)) {
		void *pd_page = phys_page_alloc();
		memset(pd_page, 0, PAGE_SIZE);
		pdpt->entry[pdpt_idx] = (u64) pd_page & ADDR_MASK | FLAGS;
		map_page(pd_page, pd_page);
	}

	struct table *pd = (void *) (pdpt->entry[pdpt_idx] & ADDR_MASK);
	if (!(pd->entry[pd_idx] & FLAG_PRESENT)) {
		void *pt_page = phys_page_alloc();
		memset(pt_page, 0, PAGE_SIZE);
		pd->entry[pd_idx] = (u64) pt_page & ADDR_MASK | FLAGS;
		map_page(pt_page, pt_page);
	}

	struct table *pt = (void *) (pd->entry[pd_idx] & ADDR_MASK);
	pt->entry[pt_idx] = (u64) phys & ADDR_MASK | FLAGS;
}

void *find_physaddr(void *virt) {
	unsigned int pml4_idx = ((u64) virt >> 39) & 0x1FF;
	unsigned int pdpt_idx = ((u64) virt >> 30) & 0x1FF;
	unsigned int pd_idx   = ((u64) virt >> 21) & 0x1FF;
	unsigned int pt_idx   = ((u64) virt >> 12) & 0x1FF;

	if (!(pml4.entry[pml4_idx] & FLAG_PRESENT))
		return 0;

	struct table *pdpt = (void *) (pml4.entry[pml4_idx] & ADDR_MASK);
	if (!(pdpt->entry[pdpt_idx] & FLAG_PRESENT))
		return 0;

	struct table *pd = (void *) (pdpt->entry[pdpt_idx] & ADDR_MASK);
	if (!(pd->entry[pd_idx] & FLAG_PRESENT))
		return 0;

	struct table *pt = (void *) (pd->entry[pd_idx] & ADDR_MASK);
	if (!(pt->entry[pt_idx] & FLAG_PRESENT))
		return 0;

	return (void *) ((pt->entry[pt_idx] & ADDR_MASK) + ((u64) virt & 0xFFF));
}

void init_paging() {
	u64 i, j;
	void *addr1, *addr2;

	/* Clear pml4 */
	memset(&pml4, 0, sizeof(pml4));

	/* Map kernel to its vaddr */
	addr1 = kernel_paddr;
	addr2 = kernel_vaddr;
	while (addr2 < kernel_vaddr + kernel_size) {
		map_page(addr1, addr2);
		addr1 += PAGE_SIZE;
		addr2 += PAGE_SIZE;
	}

	/* Map stack */
	map_page(kernel_stack_paddr, kernel_stack - PAGE_SIZE);

	/* Map framebuffer to 0x8000000000 */
	addr1 = fb_addr;
	addr2 = (void *) 0x8000000000;
	while (addr1 < ((void *) fb_addr + fb_size)) {
		map_page(addr1, addr2);
		addr1 += PAGE_SIZE;
		addr2 += PAGE_SIZE;
	}
	fb_addr = (void *) 0x8000000000;

	/* Map pmem_bmap to one page after heap */
	addr1 = pmem_bmap;
	addr2 = kernel_heap + PAGE_SIZE;
	while (addr1 < (void *) pmem_bmap + pmem_bmap_pages) {
		map_page(addr1, addr2);
		addr1 += PAGE_SIZE;
		addr2 += PAGE_SIZE;
	}

	/* Map freemap to the start of the heap */
	map_page(freemap, kernel_heap);

	/* Load the PML4 into CR3 */
	load_pml4(find_physaddr(&pml4));

	/* Change the old physical addresses to new virutal ones */
	freemap = kernel_heap;
	pmem_bmap = kernel_heap + PAGE_SIZE;
}
