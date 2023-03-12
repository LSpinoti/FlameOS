#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <cpu/IO.h>
#include <syscall.h>
#include <FS.h>
#include "FAT.h"

#define PRIMARY	0x1F0
#define SECOND	0x170

#define DAT		0
#define ERR		1
#define FEA		1
#define	SEC		2
#define LBALO	3
#define LBAMID	4
#define LBAHI	5
#define DHR		6
#define STS		7
#define CMD		7

#define ERROR	0x01
#define IDX		0x02
#define CORR	0x04
#define DRQ		0x08
#define SRV		0x10
#define DF		0x20
#define RDY		0x40
#define BSY		0x80

#define READ		0x24
#define WRITE		0x34
#define IDENTIFY	0xEC

#define DISKS 4

#define poll(status) while (inb(port + STS) & status)			/* Wait for X to clear */
#define antipoll(status) while (!(inb(port + STS) & status))	/* Wait for X to set */

FILE *files;
diskdata *disktable;
volume_t *volumes;

void smalldelay() {
	for (u8 i = 0; i < 15; ++i)
		inb(PRIMARY + STS);
}

void readsectorpio(u64 LBA, u16 count, void *buffer, unsigned int disk) {
	u16 *buf = (u16*) buffer;

	u16 port = PRIMARY;
	u8 outcmd = 0x40;

	if (disk >= 2)
		port = SECOND;

	if (disk % 2)
		outcmd = 0x50;

	poll(BSY);

	outb(port + DHR, outcmd);
	outb(port + SEC, (u8) (count >> 8));
	outb(port + LBALO, (u8) (LBA >> 24));
	outb(port + LBAMID, (u8) (LBA >> 32));
	outb(port + LBAHI, (u8) (LBA >> 40));
	outb(port + SEC, (u8) count);
	outb(port + LBALO, (u8) LBA);
	outb(port + LBAMID, (u8) (LBA >> 8));
	outb(port + LBAHI, (u8) (LBA >> 16));
	outb(port + CMD, READ);

	// TODO: allow count size one to be 0x100000 sectors
	for (u16 i = 0; i < count; ++i) {
		smalldelay();
		poll(BSY);
		antipoll(DRQ);
		for (u16 j = 0; j < 256; ++j) {
			*buf = inw(port + DAT);
			++buf;
		}
	}
}


void writesectorpio(u64 LBA, u16 count, void *buffer, unsigned int disk) {
	u16 *buf = (u16*) buffer;

	u16 port = PRIMARY;
	u8 outcmd = 0x40;

	if (disk >= 2)
		port = SECOND;

	if (disk % 2)
		outcmd = 0x50;

	poll(BSY);

	outb(port + DHR, outcmd);
	outb(port + SEC, (u8) (count >> 8));
	outb(port + LBALO, (u8) (LBA >> 24));
	outb(port + LBAMID, (u8) (LBA >> 32));
	outb(port + LBAHI, (u8) (LBA >> 40));
	outb(port + SEC, (u8) count);
	outb(port + LBALO, (u8) LBA);
	outb(port + LBAMID, (u8) (LBA >> 8));
	outb(port + LBAHI, (u8) (LBA >> 16));
	outb(port + CMD, WRITE);

	for (u16 i = 0; i < count; ++i) {
		smalldelay();
		poll(BSY);
		antipoll(DRQ);
		for (u16 j = 0; j < 256; ++j) {
			outb(port + DAT, *buf);
			++buf;
		}
	}
}

void init_disk() {
	disktable = malloc(sizeof(diskdata) * DISKS);
	volumes = malloc(sizeof(volume_t) * LETTERS); // Usually 26

	u16 port, i, j, volume_letter = 0;
	u8 outcmd;
	u8 buffer[512]; /* Ambiguously named buffer since it is used for MBR and BPB */

	for (unsigned int disk = 0; disk < 4; ++disk) {
	/* TODO: Don't assume LBA48 is supported on the drive */
		port = PRIMARY;
		outcmd = 0xA0;

		if (disk >= 2)
			port = SECOND;

		if (disk % 2)
			outcmd = 0xB0;

		poll(BSY);
		outb(port + DHR, outcmd);
		outb(port + SEC, 0);
		outb(port + LBALO, 0);
		outb(port + LBAMID, 0);
		outb(port + LBAHI, 0);
		outb(port + CMD, IDENTIFY);

		for (i = 0; i < 100; ++i) inw(port + DAT); /* Skip 100 inws */

		for (i = 0; i < 64; i += 16) /* Read 4 inws to get disk size in LBA */
			disktable[disk].blocks |= (u64) inw(port + DAT) << i;

		for (i = 0; i < 152; ++i) inw(port + DAT); /* Finish the next 152 inws */

		if (disktable[disk].blocks == 0) continue;

		/* Read MBR and store partitions to table */

		readsectorpio(0, 1, buffer, disk);

		j = 0x1BE;

		for (i = 0; i < 4; ++i) {
			disktable[disk].parts[i].loc = buffer[j + 0x8] | buffer[j + 0x9] << 8 | buffer[j + 0xA] << 16 | buffer[j + 0xB] << 24;
			if ((disktable[disk].parts[i].size = buffer[j + 0xC] | buffer[j + 0xD] << 8 | buffer[j + 0xE] << 16 | buffer[j + 0xF] << 24) != 0) {
				volumes[volume_letter].disk = disk;
				volumes[volume_letter].partition = i;
				++volume_letter;
			}
			j += 16;
		}

		/* Detect filesystems */

		for (i = 0; i < 4; ++i) {
			readsectorpio(disktable[disk].parts[i].loc, 1, buffer, disk);
			if (buffer[0x42] == 0x28 || buffer[0x42] == 0x29) { /* FAT32 detected */
				fat_setup(disk, i);
				continue;
			}
		}
	}

	files = malloc(sizeof(FILE) * MAX_OPEN_FILES);
	memset(files, 0, sizeof(FILE) * MAX_OPEN_FILES);
}
