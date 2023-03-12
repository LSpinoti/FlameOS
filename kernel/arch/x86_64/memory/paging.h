#ifndef PAGING_H
#define PAGING_H
#include <types.h>
#include <gfx.h>
#include <memory.h>
#include <globals.h>

#define FLAG_PRESENT (1 << 0)
#define FLAG_RW (1 << 1)
#define FLAG_US (1 << 2)
#define FLAGS (FLAG_PRESENT | FLAG_RW | FLAG_US)
#define ADDR_MASK 0x000ffffffffff000

struct table pml4;

extern void load_pml4(struct table *pml4);

#endif
