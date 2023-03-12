#ifndef IDT_H
#define IDT_H
#include <types.h>

void addirq(u8 entry, void *offset, u8 type);
void init_idt();
void remappic(u8 master, u8 slave);
__attribute__((no_caller_saved_registers)) void piceoi(bool isslave);
void maskirq(u8 irq);
void unmaskirq(u8 irq);

#endif
