CC := x86_64-w64-mingw32-gcc
AS := nasm
CFLAGS := -ffreestanding -c -isystem include/ -isystem ../libs/include/ -mgeneral-regs-only
ASFLAGS := -fwin64
LDFLAGS := -nostdlib -Wl,-dll -shared -Wl,--subsystem,10

OBJS :=\
boot.o \
../kernel/kernel.o

LINK_LIST := $(OBJS) $(LIBS) $(EXTRA)

.PHONY: clean

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

bootx64.efi: $(OBJS)
	$(CC) $(LDFLAGS) -e boot -o ../$@ $^

clean:
	rm -rf ../bootx64.efi *.o */*.o */*/*.o */*/*/*.o */*/*/*/*.o
