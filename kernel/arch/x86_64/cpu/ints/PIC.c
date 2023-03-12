#include <types.h>
#include <cpu/IO.h>

#define PIC1_CMD 0x20
#define PIC1_DAT 0x21
#define PIC2_CMD 0xA0
#define PIC2_DAT 0xA1
#define EOI 0x20

__attribute__((no_caller_saved_registers)) void piceoi(bool isslave) {
	if (isslave)
		outb(PIC2_CMD, EOI);
	outb(PIC1_CMD, EOI);
}

void remappic(u8 master, u8 slave) {
	u8 mm, sm; // Master mask and slave masks
	mm = inb(PIC1_DAT);
	sm = inb(PIC2_DAT);

	outb(PIC1_CMD, 0x11); // Start listening for words
	iowait();
	outb(PIC2_CMD, 0x11);
	iowait();
	outb(PIC1_DAT, master);
	iowait();
	outb(PIC2_DAT, slave);
	iowait();
	outb(PIC1_DAT, 4);
	iowait();
	outb(PIC2_DAT, 2);
	iowait();
	outb(PIC1_DAT, 1);
	iowait();
	outb(PIC2_DAT, 1);
	iowait();

	// Restore masks
	outb(PIC1_DAT, mm);
	outb(PIC2_DAT, sm);
}

void maskirq(u8 irq) {
	if (irq < 8) {
		outb(PIC1_DAT, inb(PIC1_DAT) | (1 << irq));
	}
	else {
		outb(PIC2_DAT, inb(PIC2_DAT) | (1 << (irq - 8)));
	}
}

void unmaskirq(u8 irq) {
	if (irq < 8)
		outb(PIC1_DAT, inb(PIC1_DAT) & ~(1 << irq));
	else
		outb(PIC2_DAT, inb(PIC2_DAT) & ~(1 << (irq - 8)));
}
