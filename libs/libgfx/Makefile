CC := x86_64-elf-gcc
AR := x86_64-elf-ar
CFLAGS := -ffreestanding -c  -isystem ../include/ -mcmodel=large -g -mno-red-zone

OBJS :=\
gfx/fillrect.o \
gfx/plotimg.o \
gfx/plotpixel.o \
img/tga.o

.PHONY: clean

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

libgfx.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

clean:
	rm -rf libgfx.a */*.o
