#include <types.h>

size_t wcstombs(char *dest, const wchar *src, size_t max) {
	size_t count = 0;
	while (max) {
		if ((*dest++ = (char) *src++) == '\0') return count;
		--max;
		++count;
	}

	return -1;
}

void wcatomba(u8 *dest, const wchar *src, size_t count) {
	while (count) {
		*dest++ = (char) *src++;
		--count;
	}
}
