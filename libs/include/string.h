#ifndef STRING_H
#define STRING_H
#include <types.h>

int memcmp(const void *ptr1, const void *ptr2, size_t len);
void* memcpy(void *dest, void *src, size_t size);
void *memset(void *ptr, int value, size_t size);
int strcmp(const char *str1, const char *str2);
char* strcpy(char *dest, const char *src);

#endif
