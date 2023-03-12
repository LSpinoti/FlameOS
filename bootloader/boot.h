#ifndef EXTRA_H
#define EXTRA_H

#define PAGE_SIZE 4096
#define CEILING(a, b)(((a)+(b)-1)/(b))

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef unsigned long long size_t;

typedef signed short wchar;

#define NULL ((void*)0)

void memcpy(void *dest, const void *src, size_t len) {
	u8 *d = dest;
	const u8 *s = src;

	while (len) {
		*d++ = *s++;
		--len;
	}
}

void memzero(void *mem, size_t len) {
	u8 *m = mem;
	while (len--) *m++ = 0;
}

int strcmp(const char *str1, const char *str2) {
	while (*str1 == *str2) {
		if (*str1 == '\0') return 0;
		++str1;
		++str2;
	}
	if (*str1 > *str2) return 1;
	else return -1;
}

extern int _binary_kernel_start;
extern int _binary_kernel_end;
extern int _binary_kernel_size;

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
} memory_t;

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

bootinfo_t bootinfo;

void map_page(void *phys, void *virt);

#endif
