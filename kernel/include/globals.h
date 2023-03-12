#ifndef GLOBALS_H
#define GLOBALS_H
#include <types.h>
#include <memory.h>
#include <gfx.h>

// Defined in main.h
extern memmap_t *memmap;
extern size_t mm_descs;
extern size_t mm_desc_size;

extern memory_t *freemap;
extern size_t fm_descs;

extern size_t memory_pages;
extern u32 *pmem_bmap;
extern size_t pmem_bmap_pages;

extern void *kernel_paddr;
extern void *kernel_vaddr;
extern size_t kernel_size;

extern void *kernel_stack_paddr;

extern void *efi_rs;

extern pixel_t *fb_addr;
extern size_t fb_size;
extern u32 fb_w;
extern u32 fb_h;
extern u32 fb_pixperline;

// Defined in mem/mem.h
extern void *const kernel_heap;
extern void *const kernel_stack;
extern void *const userspace_heap;
extern void *const userspace_stack;

extern u32 *kheap_bmap;
extern u32 *uheap_bmap;

extern size_t kheap_bmap_pages;
extern size_t uheap_bmap_pages;

struct table {
	u64 entry[512];
} __attribute__((aligned(4096))) extern pml4;

#endif
