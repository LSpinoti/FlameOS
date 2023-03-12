#include <types.h>
#include <memory.h>
#include <string.h>

#define ENTRIES 8

typedef struct {
	u16 size;
	void *offset;
} __attribute__((packed)) gdt_ptr;
gdt_ptr GDTP;

typedef struct {
	u16 limit;
	u16 base1;
	u8 base2;
	u8 access;
	u8 flags;
	u8 base3;
} __attribute__((packed)) gdt_entry;
gdt_entry GDT[ENTRIES];
u8 GDT_entry = 0;

typedef struct {
	u32 resv1;
	void *RSP0;
	void *RSP1;
	void *RSP2;
	u64 resv2;
	void *IST1;
	void *IST2;
	void *IST3;
	void *IST4;
	void *IST5;
	void *IST6;
	void *IST7;
	u64 resv3;
	u16 resv4;
	u16 IOPB;
} __attribute__((packed)) TSS_t;
TSS_t TSS;

void flushregs();

void gdtadd(u32 base, u32 size, u8 access, bool longcode) {
	gdt_entry *sector = &GDT[GDT_entry];

	size >>= 12;

	sector->base1 = base & 0xFFFF;
	sector->base2 = (base >> 16) & 0xFF;
	sector->base3 = (base >> 24) & 0xFF;

	sector->limit = size & 0xFFFF;

	if (longcode)
		sector->flags = 0xA0 | ((size >> 16) & 0x0F);
	else
		sector->flags = 0xC0 | ((size >> 16) & 0x0F);
	sector->access = access;
	++GDT_entry;
}

void init_tss() {
//	memset(&TSS, 0, sizeof(TSS));
//	TSS.IOPB = sizeof(TSS);
//	IST1_addr = alloc_page() + PAGE_SIZE;
//	TSS.IST1 = &IST1_addr;

	u64 *entry = (u64 *) &GDT[GDT_entry];

	*entry = 0;

//	*entry |= sizeof(TSS);
//	*entry |= ((u64) &TSS & 0xFFFFFF) << 16;
	*entry |= (u64) 0x89 << 40;
	*entry |= (u64) 0x40 << 48;
//	*entry |= ((u64) &TSS & 0xFF000000) << 56;
	++entry;
//	*entry |= (u64) &TSS >> 32;
	GDT_entry += 2;
}

void init_gdt() {
	// TODO: Setup actual sections
	gdtadd(0, 0, 0, FALSE);					// Null			0x00
	gdtadd(0, 0xFFFFFFFF, 0x9A, TRUE);		// Code			0x08
	gdtadd(0, 0xFFFFFFFF, 0x92, FALSE);		// Data			0x10
	gdtadd(0, 0, 0, FALSE);					// User Base	0x18
	gdtadd(0, 0xFFFFFFFF, 0xF2, FALSE);		// User Data	0x20
	gdtadd(0, 0xFFFFFFFF, 0xFA, TRUE);		// User Code	0x28

//	init_tss();								// TSS			0x30

	GDTP.offset = &GDT;
	GDTP.size = sizeof(GDT) - 1;

	__asm__("lgdt (%0)" :: "r"(&GDTP));
	flushregs();
}
