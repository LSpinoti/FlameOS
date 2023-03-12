#include <gfx.h>
#include <stdlib.h>
#include <FS.h>
#include <stdio.h>
#include <cpu/GDT.h>
#include <cpu/IO.h>
#include <cpu/IDT.h>
#include <cpu/PIT.h>
#include <memory.h>
#include <types.h>
#include "init.h"
#include <syscall.h>

void init_syscall();
void *malloc_heap(size_t bytes, int heap);
void memdump(const void *ptr, size_t count);

void init() {
	bootinfo_t *bootinfo;

	__asm__ ("movq %%rdi, %0" : "=r"(bootinfo));
	init_globals(bootinfo);

	debug_log(
	"GLOBALS:\n"
	"memmap: %p\n"
	"mm_descs: %d\n\n"
	"freemap: %p\n"
	"fm_descs: %d\n\n"
	"kernel_paddr: %p\n"
	"kernel_vaddr: %p\n"
	"kernel_size: %d\n\n"
	"memory_bitmap: %p\n"
	"memory_pages: %d\n\n"
	"fb_addr: %p\n"
	"fb_size: 0x%X\n"
	"fb_w: %d\n"
	"fb_h: %d\n"
	"fb_pixperline: %d\n\n\n",
	memmap, mm_descs, freemap, fm_descs,
	kernel_paddr, kernel_vaddr, kernel_size,
	pmem_bmap, memory_pages,
	fb_addr, fb_size, fb_w, fb_h, fb_pixperline);

	for (size_t i = 0; i < fm_descs; ++i) {
		debug_log(
		"%d:\n"
		"Addr: %p\n"
		"Pages: 0x%X\n\n",
		i, freemap[i].addr, freemap[i].blocks);
	}

	init_gdt();
	init_memory();
	init_idt();
	init_timer();
//	init_disk();
//	init_syscall();

	int *addr = malloc(sizeof(int) * 40000);
	for (int i = 0; i < 40000; ++i)
		addr[i] = i;
	for (int i = 0; i < 40000; ++i)
		debug_log("%d\n", addr[i]);

	debug_log("ADDR: %p\n", addr);
	int *addr2 = malloc(sizeof(int) * 5);
	debug_log("ADDR2: %p\n", addr2);
//	free(addr);

	fillrect(0,0, fb_w, fb_h, 0xc99eff);
	for (int i = 0; i <= fb_h; ++i)
		fillrect(0, 0, i, i, 0x006040);
	for (;;);
}
