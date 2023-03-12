#include <types.h>
#include <syscall.h>
#include <cpu/IDT.h>
#include <cpu/IO.h>

#define CHAN0	0x40
#define CHAN1	0x41
#define CHAN2	0x42
#define CMD		0x43

#define HZ 1000

u64 timerticks;

void pitirq();

void init_timer() {
	u32 div = 1193180 / HZ;
	outb(CMD, 0x36);
	outb(CHAN0, div & 0xFF);
	outb(CHAN0, div >> 8);
	addirq(0x20, pitirq, 0x8E);
	unmaskirq(0);
}

void sleep(unsigned long ticks) {
	for (unsigned long exit = timerticks + ticks; timerticks < exit;);
}
