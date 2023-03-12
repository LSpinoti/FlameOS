#ifndef MAIN_H
#define MAIN_H
#include <types.h>
#include <memory.h>
#include <gfx.h>
#include <macros.h>

typedef struct {
	memmap_t *memmap;
	size_t mm_descs;

	memory_t *freemap;
	size_t fm_descs;

	void *k_paddr;
	void *k_vaddr;
	size_t k_size;
	void *k_stack_paddr;

	void *mb_addr;
	size_t mb_pages;

	void *efi_rs;

	void *fb_addr;
	size_t fb_size;
	u32 fb_w;
	u32 fb_h;
	u32 fb_pixperline;
} bootinfo_t;

typedef size_t EFI_STATUS;

typedef struct {
	u64 Signature;
	u32 Revision;
	u32 HeaderSize;
	u32 CRC32;
	u32 Reserved;
} EFI_TABLE_HEADER;

typedef struct {
	EFI_TABLE_HEADER Hdr;
	EFI_STATUS (*GetTime)();
	EFI_STATUS (*SetTime)();
	EFI_STATUS (*GetWakeupTime)();
	EFI_STATUS (*SetWakeupTime)();
	EFI_STATUS (*SetVirtualAddressMap)(size_t MemoryMapSize, size_t DescriptorSize, u32 DescriptorVersion, memmap_t *VirtualMap);
	EFI_STATUS (*ConvertPointer)();
	EFI_STATUS (*GetVariable)();
	EFI_STATUS (*GetNextVariableName)();
	EFI_STATUS (*SetVariable)();
	EFI_STATUS (*GetNextHighMonotonicCount)();
	EFI_STATUS (*ResetSystem)();
	EFI_STATUS (*UpdateCapsule)();
	EFI_STATUS (*QueryCapsuleCapabilities)();
	EFI_STATUS (*QueryVariableInfo)();
} EFI_RS;

memmap_t *memmap;
size_t mm_descs;
size_t mm_desc_size;

memory_t *freemap;
size_t fm_descs;

size_t memory_pages;
u32 *pmem_bmap;
size_t pmem_bmap_pages;

void *kernel_paddr;
void *kernel_vaddr;
size_t kernel_size;

void *kernel_stack_paddr;

void *efi_rs;

pixel_t *fb_addr;
size_t fb_size;
u32 fb_w;
u32 fb_h;
u32 fb_pixperline;

void init_globals(bootinfo_t *b) {
	memmap			= b->memmap;
	mm_descs		= b->mm_descs;

	freemap			= b->freemap;
	fm_descs		= b->fm_descs;

	kernel_paddr	= b->k_paddr;
	kernel_vaddr	= b->k_vaddr;
	kernel_size		= b->k_size;

	kernel_stack_paddr = b->k_stack_paddr;

	pmem_bmap		= b->mb_addr;
	memory_pages	= b->mb_pages;
	pmem_bmap_pages	= CEILING(memory_pages, (8 * 4096 * 4096));

	efi_rs			= b->efi_rs;

	fb_addr			= b->fb_addr;
	fb_size			= b->fb_size;
	fb_w			= b->fb_w;
	fb_h			= b->fb_h;
	fb_pixperline	= b->fb_pixperline;
}

#endif
