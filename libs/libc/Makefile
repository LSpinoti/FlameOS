CC := x86_64-elf-gcc
AR := x86_64-elf-ar
CFLAGS := -ffreestanding -c -isystem ../include/ -isystem ../../kernel/include -mcmodel=large -g -mno-red-zone

OBJS :=\
stdlib/itoa.o \
stdlib/wcstombs.o \
string/memcmp.o \
string/memcpy.o \
string/memset.o \
string/strcmp.o \
string/strcpy.o

.PHONY: clean

%.o: %.c
	$(CC) -o $@ $< $(CFLAGS)
	
libk.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

clean:
	rm -rf libk.a */*.o
