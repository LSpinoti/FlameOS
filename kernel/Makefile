ARCH := x86_64
CC := $(ARCH)-elf-gcc
LD := $(ARCH)-elf-ld
AS := $(ARCH)-elf-as
CFLAGS := -g -ffreestanding -c -isystem include/ -isystem ../libs/include/ -mgeneral-regs-only -mcmodel=large -mno-red-zone
ASFLAGS := -felf64
LDFLAGS := -g -nostdlib -ffreestanding -z max-page-size=4096 -T linker.ld

OBJS :=\
arch/$(ARCH)/init.o \
arch/$(ARCH)/cpu/GDT/GDT.S.o \
arch/$(ARCH)/cpu/GDT/GDT.o \
arch/$(ARCH)/cpu/IO.o \
arch/$(ARCH)/cpu/ints/IDT.o \
arch/$(ARCH)/cpu/ints/PIC.o \
arch/$(ARCH)/cpu/ints/timer/PIT.S.o \
arch/$(ARCH)/cpu/ints/timer/PIT.o \
memory/memory.o \
arch/$(ARCH)/memory/paging.as.o \
arch/$(ARCH)/memory/paging.o \
arch/$(ARCH)/syscall/debug_log.o \
arch/$(ARCH)/syscall/syscall.o \
term/terminal.o

LIBDIR := ../libs
LIBS :=\
$(LIBDIR)/libc/libk.a \
$(LIBDIR)/libgfx/libgfx.a

EXTRA :=\
data/font.dat.o

LINK_LIST := $(OBJS) $(LIBS) $(EXTRA)

.PHONY: clean

%.as.o: %.asm
	nasm -felf64 $< -o $@

%.S.o: %.S
	$(AS) -o $@ $<

%.dat.o: %.psfu
	objcopy -O elf64-x86-64 -B i386 -I binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

kernel: $(LINK_LIST)
	$(CC) $(LDFLAGS) -o $@ $^
	objcopy -O pe-x86-64 -B i386 -I binary kernel kernel.o

clean:
	rm -rf ../bootx64.efi *.o */*.o */*/*.o */*/*/*.o */*/*/*/*.o */*/*/*/*/*.o  kernel
