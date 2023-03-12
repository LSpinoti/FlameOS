#ifndef STDLIB_H
#define STDLIB_H
#include <types.h>

void *malloc(size_t size);
char *itoa(unsigned long long num, char *out, unsigned int base);
unsigned int getdigits(unsigned long long num, unsigned int base);
void* pmalloc(size_t bytes);
void pfree(void *addr);
size_t wcstombs(char *dest, const wchar *src, size_t max);
void wcatomba(u8 *dest, const wchar *src, size_t count);

#endif
