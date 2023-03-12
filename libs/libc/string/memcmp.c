#include <types.h>

int memcmp(const void *ptr1, const void *ptr2, size_t len) {
	unsigned char *c1 = (unsigned char*) ptr1;
	unsigned char *c2 = (unsigned char*) ptr2;

	while (len) {
		if (*c1 != *c2) {
			if (*c1 > *c2) return 1;
			else return -1;
		}
		--len;
	}

	return 0;
}
