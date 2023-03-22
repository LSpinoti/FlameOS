QEMU_FLAGS := -bios /usr/share/ovmf/x64/OVMF.fd -kernel bootx64.efi -net none -m 2G
#-drive file=disk.img,format=raw
QEMU_DEBUG_FLAGS := $(QEMU_FLAGS) -S -s
LIBS :=\
libs/libgfx/libgfx.a

.PHONY: all kernel libs run debug clean

all: bootloader kernel libs

bootloader: kernel
	make -C bootloader

kernel: libs
	make -C kernel

libs:
	make -C libs

run:
	qemu-system-x86_64 $(QEMU_FLAGS)

debug:
	qemu-system-x86_64 $(QEMU_DEBUG_FLAGS)

install: kernel
	cp bootx64.efi /boot/flame

clean:
	rm -f bootx64.efi
	make clean -C kernel
	make clean -C libs
