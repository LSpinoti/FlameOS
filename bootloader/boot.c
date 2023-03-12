#include "boot.h"
#include "ELF.h"
#include "uefi.h"

typedef struct {
	u64 entry[512];
} __attribute__((aligned (4096))) P_table;

P_table PML4;
systable *system_table;

void boot(handle imghandle, systable *systab) {
	size_t i, j;
	system_table = systab;

	/* Get memmap and memkey */
	size_t memmapsize = 0;
	size_t mm_desc_size;
	u64 mapkey;
	u32 descver;

	/*
		- Check the size of the memory map
		- Allocate that size
		- Retrieve the map
	*/
	systab->bs->getmemmap(&memmapsize, NULL, &mapkey, &mm_desc_size, &descver);
	systab->bs->pagealloc(any, rsdatamem, CEILING(memmapsize, PAGE_SIZE), &bootinfo.memmap);
	systab->bs->getmemmap(&memmapsize, bootinfo.memmap, &mapkey, &mm_desc_size, &descver);
	bootinfo.mm_descs = memmapsize / mm_desc_size;

	/*
		Shrink the memory map to 40-byte descriptors.
		This assumes mm_desc_size >= normal memmap_t size.
		If it isn't, we have a larger problem. :]
	*/
	if (mm_desc_size != sizeof(memmap_t)) {
		void *addr1, *addr2;
		addr1 = addr2 = bootinfo.memmap;
		for (i = 0; i < bootinfo.mm_descs; ++i) {
			memcpy(addr1, addr2, sizeof(memmap_t));
			addr1 += sizeof(memmap_t);
			addr2 += mm_desc_size;
		}
	}


	/* Initialize Graphics Output Protocol (GOP) */
	guid gopguid = GOPGUID;
	gop *gop;
	systab->bs->locprotocol(&gopguid, NULL, (void **) &gop);
	u32 maxmodes = gop->mode->maxmode;
	gopmodeinfo *tmp;
	u64 infosize;
	u64 res;
	u64 bestres = 0;
	u32 bestmode = 0;

	for (i = 0; i < maxmodes; ++i) {
		gop->querymode(gop, i, &infosize, &tmp);
		res = tmp->w + tmp->h;
		if (res > bestres) {
			bestres = res;
			bestmode = i;
		}
	}

	gop->setmode(gop, bestmode);

	bootinfo.fb_addr = gop->mode->fb;
	bootinfo.fb_size = gop->mode->fbsize;
	bootinfo.fb_w = gop->mode->info->w;
	bootinfo.fb_h = gop->mode->info->h;
	bootinfo.fb_pixperline = gop->mode->info->pixperline;	




	/* Load ELF kernel */
	void *const kfile = &_binary_kernel_start;
	void *kernel;
	void *const kernel_vaddr = (void *) 0x1000000000;
	elf_fheader_t *fheader = kfile;
	elf_pheader_t *pheader = kfile + fheader->e_phoff;
	size_t kernel_mem_size = 0;
	size_t lowest_vaddr = 0xFFFFFFFFFFFFFFFF;

	/* Verify it is an ELF file */
	if (*((u32 *) fheader) != 0x464c457f) return;

	/* Find the amount of memory that must be allocated */
	for (i = 0; i < fheader->e_phnum; ++i) {
	if (pheader[i].p_type == PT_LOAD) {
		if (pheader[i].p_vaddr + pheader[i].p_memsz > kernel_mem_size)
			kernel_mem_size = pheader[i].p_vaddr + pheader[i].p_memsz;
		if (pheader[i].p_vaddr < lowest_vaddr)
			lowest_vaddr = pheader[i].p_vaddr;
		}
	}
	kernel_mem_size -= lowest_vaddr;

	systab->bs->pagealloc(any, rscodemem, CEILING(kernel_mem_size, PAGE_SIZE), &kernel);
	bootinfo.k_paddr = kernel;
	bootinfo.k_vaddr = kernel_vaddr;
	bootinfo.k_size = kernel_mem_size;

	/* Load the segments into memory */
	for (i = 0; i < fheader->e_phnum; ++i) {
		if (pheader[i].p_type == PT_LOAD) {
			memcpy(kernel + (pheader[i].p_vaddr - (size_t) kernel_vaddr) , kfile + pheader[i].p_offset, pheader[i].p_filesz);
			memzero(kernel + (pheader[i].p_vaddr - (size_t) kernel_vaddr) + pheader[i].p_filesz, pheader[i].p_memsz - pheader[i].p_filesz);
		}
	}


	/* Count freemap descs */
	bootinfo.fm_descs = 0;
	for (i = 0; i < bootinfo.mm_descs; ++i)
		if (bootinfo.memmap[i].type == normalmem) ++bootinfo.fm_descs;

	/* Allocate memory for freemap */
	systab->bs->pagealloc(any, rscodemem, CEILING(bootinfo.fm_descs * sizeof(memory_t), PAGE_SIZE), &bootinfo.freemap);

	/* Reread the memmap in case allocating pages took memory */
	systab->bs->getmemmap(&memmapsize, bootinfo.memmap, &mapkey, &mm_desc_size, &descver);

	/* Fill freemap */
	j = 0;
	for (i = 0; i < bootinfo.mm_descs; ++i)
		if (bootinfo.memmap[i].type == normalmem) {
			bootinfo.freemap[j].addr = bootinfo.memmap[i].physaddr;
			bootinfo.freemap[j].blocks = bootinfo.memmap[i].pages;
			++j;
		}

	/* Set up temporary paging structure */
	memzero(&PML4, sizeof(PML4));

	/* Identity map non-conv memory */
	void *a;
	size_t b;
	for (i = 0; i < bootinfo.mm_descs; ++i) {
		if (bootinfo.memmap[i].type == 7) continue;
		a = bootinfo.memmap[i].physaddr;
		for (j = 0; j < bootinfo.memmap[i].pages; ++j) {
			map_page(a, a);
			a += PAGE_SIZE;
		}
	}

	/* ID map first 16MB */
	for (a = 0; a < (void *) 0x1000000; a += PAGE_SIZE)
		map_page(a, a);

	/* Map kernel to its vaddr */
	a = kernel;
	b = 0;
	do {
		map_page(a, (void *) (kernel_vaddr + b));
		a += PAGE_SIZE;
		b += PAGE_SIZE;
	} while (b < kernel_mem_size);

	/* Create space for memory_bitmap, then ID map it */
	bootinfo.mb_pages = 0;
	for (i = 0; i < bootinfo.fm_descs; ++i)
		bootinfo.mb_pages += bootinfo.freemap[i].blocks;
	systab->bs->pagealloc(any, rscodemem, CEILING(bootinfo.mb_pages, 32768), &bootinfo.mb_addr);
	for (a = bootinfo.mb_addr; a < bootinfo.mb_addr + CEILING(bootinfo.mb_pages, 32768); a += PAGE_SIZE)
		map_page(a, a);
	memzero(bootinfo.mb_addr, CEILING(bootinfo.mb_pages, 8));

	/* Create kernel stack (virtual 0x2000000000) */
	systab->bs->pagealloc(any, rsdatamem, 1, &bootinfo.k_stack_paddr);

	void *const kernel_stack = (void *) 0x2000000000;
	map_page(bootinfo.k_stack_paddr, kernel_stack - PAGE_SIZE);


	bootinfo.efi_rs = systab->rs;


	/* Exit UEFI */
	systab->bs->exitbs(imghandle, mapkey);

	/* Load CR3 with new paging structure */
	__asm__(
	"mov %0, %%rdi\n"
	"mov $0x000ffffffffff000, %%rax\n"
	"and %%rax, %%rdi\n"
	"mov %%rdi, %%cr3"
	:: "r"(&PML4) : "rdi", "rax");

	/* ...ENTER TEH KERNEL!!! */
	__asm__ (
	"mov %2, %%rsp\n"
	"mov %1, %%rdi\n"
	"jmp %0"
	:: "r"((void *)fheader->e_entry), "r"(&bootinfo), "r"(kernel_stack): "rdi");
}





#define FLAG_PRESENT (1 << 0)
#define FLAG_RW (1 << 1)
#define FLAG_US (1 << 2)
#define FLAGS (FLAG_PRESENT | FLAG_RW | FLAG_US)
#define ADDR_MASK 0x000ffffffffff000

void map_page(void *phys, void *virt) {
	if ((u64) virt % PAGE_SIZE) return;

	unsigned int pml4_idx = ((u64) virt >> 39) & 0x1FF;
	unsigned int pdpt_idx = ((u64) virt >> 30) & 0x1FF;
	unsigned int pd_idx   = ((u64) virt >> 21) & 0x1FF;
	unsigned int pt_idx   = ((u64) virt >> 12) & 0x1FF;

	if (!(PML4.entry[pml4_idx] & FLAG_PRESENT)) {
		void* pdpt_page;
		system_table->bs->pagealloc(any, rsdatamem, 1, &pdpt_page);
		memzero(pdpt_page, PAGE_SIZE);
		PML4.entry[pml4_idx] = (u64) pdpt_page & ADDR_MASK | FLAGS;
		map_page(pdpt_page, pdpt_page);
	}

	P_table *pdpt = (void *) (PML4.entry[pml4_idx] & ADDR_MASK);
	if (!(pdpt->entry[pdpt_idx] & FLAG_PRESENT)) {
		void *pd_page;
		system_table->bs->pagealloc(any, rsdatamem, 1, &pd_page);
		memzero(pd_page, PAGE_SIZE);
		pdpt->entry[pdpt_idx] = (u64) pd_page & ADDR_MASK | FLAGS;
		map_page(pd_page, pd_page);
	}

	P_table *pd = (void *) (pdpt->entry[pdpt_idx] & ADDR_MASK);
	if (!(pd->entry[pd_idx] & FLAG_PRESENT)) {
		void *pt_page;
		system_table->bs->pagealloc(any, rsdatamem, 1, &pt_page);
		memzero(pt_page, PAGE_SIZE);
		pd->entry[pd_idx] = (u64) pt_page & ADDR_MASK | FLAGS;
		map_page(pt_page, pt_page);
	}

	P_table *pt = (void *) (pd->entry[pd_idx] & ADDR_MASK);
	pt->entry[pt_idx] = (u64) phys & ADDR_MASK | FLAGS;
}
