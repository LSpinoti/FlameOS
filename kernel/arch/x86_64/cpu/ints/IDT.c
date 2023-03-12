#include <types.h>
#include <syscall.h>
#include <cpu/IDT.h>
#include <gfx.h>
#define INTS 0x30

typedef struct {
	u16 size;
	void *offset;
} __attribute__((packed)) idt_ptr;
idt_ptr IDTP;

typedef struct {
	u16 offset1;
	u16 selector;
	u8 ist;
	u8 type;
	u16 offset2;
	u32 offset3;
	u32 resv;
} __attribute__((packed)) idt_entry;
idt_entry IDT[INTS];

/*
 * INTERRUPT HANDLERS
 */

/* A lot of these functions have a ptr argument that isn't used
 * because the compiler requires it to be there :|
 */

__attribute__((no_caller_saved_registers)) void error(const char* format, ...);

__attribute__((interrupt)) void general_fault(void *ptr) {
	error("General fault RIP :[");
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void non_maskable_interrupt(void *ptr) {
	error("Non-maskable interrupt :|");
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void div_0_fault(void *ptr) {
	error("#DE Divide by 0 error :[");
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void debug_fault(void *ptr) {
	error("#DB Debug fault :[");
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void breakpoint_fault(void *ptr) {
	error("#BP Breakpoint fault :[");
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void overflow_fault(void *ptr) {
	error("#OF Overflow fault :[");
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void bound_fault(void *ptr) {
	error("#BR Bound range exceeded :[");
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void inv_opcode_fault(void *ptr) {
	error("#UD Invalid opcode :[");
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void no_device_fault(void *ptr) {
	error("#NM Device not found :[");
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void double_fault(void *ptr) {
	error("#DF Double fault :[");
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void inv_tss_fault(void *ptr, u64 code) {
	error("#TS Invalid TSS :[ CODE: %X", code);
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void no_segment_fault(void *ptr, u64 code) {
	error("#NP Segment not present :[ CODE: %X", code);
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void stack_fault(void *ptr, u64 code) {
	error("#SS Stack segment fault :[ CODE: %X");
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void gp_fault(void *ptr, u64 code) {
	error("#GP General protection fault :[ CODE: %X", code);
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void page_fault(void *ptr, u64 code) {
	error("#PF Page fault :[ CODE: %X", code);
	while (1)
		asm ("hlt");
}

__attribute__((interrupt)) void nop_master(void *ptr) {
	piceoi(FALSE);
}

__attribute__((interrupt)) void nop_slave(void *ptr) {
	piceoi(TRUE);
}

void addirq(u8 entry, void *offset, u8 type) {
	u64 off = (u64) offset;
	idt_entry *idt = &IDT[entry];
	idt->offset1 = off & 0xFFFF;
	idt->offset2 = (off >> 16) & 0xFFFF;
	idt->offset3 = (off >> 32) & 0xFFFFFFFF;
	idt->ist = 0;
	idt->selector = 8;
	idt->type = type;
	idt->resv = 0;
}

void init_idt() {
	u8 i;
	for (i = 0; i < 0x20; ++i)
		addirq(i, general_fault, 0x8F);
	for (i = 0x20; i < 0x28; ++i)
		addirq(i, nop_master, 0x8E);
	for (i = 0x28; i < 0x30; ++i)
		addirq(i, nop_slave, 0x8E);
	addirq(0x0, div_0_fault, 0x8F);
	addirq(0x1, debug_fault, 0x8F);
	addirq(0x2, non_maskable_interrupt, 0x8F);
	addirq(0x3, breakpoint_fault, 0x8F);
	addirq(0x4, overflow_fault, 0x8F);
	addirq(0x5, bound_fault, 0x8F);
	addirq(0x6, inv_opcode_fault, 0x8F);
	addirq(0x7, no_device_fault, 0x8F);
	addirq(0x8, double_fault, 0x8F);
	addirq(0xA, inv_tss_fault, 0x8F);
	addirq(0xB, no_segment_fault, 0x8F);
	addirq(0xC, stack_fault, 0x8F);
	addirq(0xD, gp_fault, 0x8F);
	addirq(0xE, page_fault, 0x8F);

	remappic(0x20, 0x28);

	IDTP.offset = IDT;
	IDTP.size = sizeof(IDT) - 1;

	__asm__("lidt (%0)" :: "r"(&IDTP));

	__asm__("sti; nop; nop");
}
