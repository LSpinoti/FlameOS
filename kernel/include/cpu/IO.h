#ifndef CPU_H
#define CPU_H
#include <types.h>

u8 inb(u16 port);
void outb(u16 port, u8 value);
u16 inw(u16 port);
void outw(u16 port, u16 value);
u64 rdmsr(u32 msr);
void wrmsr(u32 msr, u64 value);
void iowait();

#endif
