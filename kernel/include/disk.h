#ifndef DISK_H
#define DISK_H
#include <types.h>

#define MAX_OPEN_FILES 0x100
#define SECTOR_SIZE 512
#define LETTERS 26

typedef u64 LBA;

typedef enum {
	none,
	fat32
} FS;

typedef struct {
	LBA loc;
	LBA size;
	FS fs;
	void *fsdata;
} partinfo;

typedef struct {
	u64 blocks;
	partinfo parts[4];
} diskdata;

typedef struct {
	unsigned int disk;
	unsigned int partition;
} volume_t;

void readsectorpio(u64 LBA, u16 count, void *buffer, unsigned int disk);
void writesectorpio(u64 LBA, u16 count, void *buffer, unsigned int disk);
void init_disk();

extern diskdata *disktable;
extern volume_t *volumes;

#endif
