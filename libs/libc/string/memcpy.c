#include <types.h>

void* memcpy(void *dest, void *src, size_t size) {
	u8 *chardest = dest;
	u8 *charsrc = src;
	for (size_t i = 0; i < size; ++i) {
		*chardest++ = *charsrc++;
	}
	return dest;
}
