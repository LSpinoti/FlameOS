#include <types.h>

u8 inb(u16 port) {
	u8 value;
	__asm__("inb %1, %0" : "=a" (value) : "Nd" (port));
	return value;
}

void outb(u16 port, u8 value) {
	__asm__("outb %0, %1" : : "a" (value), "Nd" (port));
}

u16 inw(u16 port) {
	u16 value;
	__asm__("inw %1, %0" : "=a" (value) : "Nd" (port));
	return value;
}

void outw(u16 port, u16 value) {
	__asm__("outw %0, %1" : : "a" (value), "Nd" (port));
}

u64 rdmsr(u32 msr) {
	u64 value;
	u32 *low = (u32*) &value;
	u32 *high = (u32*) &value + 1;
	__asm__("rdmsr" : "=a" (*low), "=r" (*high) : "c" (msr));
	return value;
}

void wrmsr(u32 msr, u64 value) {
	__asm__("wrmsr" : : "a" ((u32) value), "d" ((u32) (value >> 32)), "c" (msr));
}

void iowait() {
	outb(0x80, 0x80);
}
