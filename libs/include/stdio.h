#ifndef STDIO_H
#define STDIO_H
#include <types.h>

typedef enum {
	SEEK_SET,
	SEEK_CUR,
	SEEK_END
} fpos_t;

typedef struct {
	char **path;

	unsigned char volume;

	size_t pointer;
	size_t size; // Bytes
	unsigned char flags;
	bool used; // Is this file struct free to use
} FILE;

FILE *fopen(char *path, u8 flags);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t bytes, FILE *fp);
int fseek(FILE *fp, long long offset, fpos_t position);
void rewind(FILE *fp);
unsigned long long fsize(FILE *fp);


#endif
