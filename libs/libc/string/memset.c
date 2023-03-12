#include <types.h>

void *memset(void *ptr, int value, size_t size) {
	unsigned char *p = (unsigned char*) ptr;
	unsigned char c = (unsigned char) value;

	while (size) {
		*p++ = c;
		--size;
	}

	return ptr;
}
