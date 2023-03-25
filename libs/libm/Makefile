CC := x86_64-elf-gcc
AR := x86_64-elf-ar
CFLAGS := -ffreestanding -c -isystem ../include/ -isystem ../../kernel/include -mcmodel=large -g -mno-red-zone

OBJS :=\
math/abs.o \
math/cbrt.o \
math/ceil.o \
math/cos.o \
math/exp.o \
math/fabs.o \
math/floor.o \
math/fmod.o \
math/hypot.o \
math/log.o \
math/pow.o \
math/sin.o \
math/sqrt.o \
math/tan.o \
math/trunc.o

%.o: %.c
	$(CC) -o $@ $< $(CFLAGS)

libm.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

clean:
	rm -rf libm.a */*.o
