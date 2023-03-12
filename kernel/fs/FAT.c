#include <types.h>
#include <syscall.h>
#include <stdlib.h>
#include <string.h>
#include <FS.h>
#include "FAT.h"

void fat_setup(unsigned int disk, unsigned int part) {
	bootrecord BPB;
	fat_fs_info fs_info;
	partinfo *info = &disktable[disk].parts[part];
	u32 FAT_buffer[SECTOR_SIZE / 4];

	readsectorpio(info->loc, 1, &BPB, disk);

	fs_info.bytes_per_sector = BPB.bytes_per_sector;
	fs_info.sectors_per_cluster = BPB.sectors_per_cluster;
	fs_info.sectors_per_fat = BPB.sectors_per_FAT;

	fs_info.root_cluster = BPB.root_cluster;
	fs_info.root_entries = BPB.root_entries;
	fs_info.root_sectors = (fs_info.root_entries * 32 + (fs_info.bytes_per_sector - 1)) / fs_info.bytes_per_sector;

	fs_info.fat_count = BPB.FAT_count;

	fs_info.fat = (BPB.reserved_sectors * fs_info.bytes_per_sector) / SECTOR_SIZE;
	fs_info.data = fs_info.fat + (fs_info.fat_count * ((fs_info.sectors_per_fat * fs_info.bytes_per_sector) / SECTOR_SIZE)) - 2;

	/* Checking how many clusters the root dir takes up */

	fs_info.root_clusters = 1;
	u32 current_cluster = fs_info.root_cluster;
	readsectorpio(info->loc + fs_info.fat + current_cluster / FEPS, 1, FAT_buffer, disk);
	do {
		if (current_cluster / FEPS != FAT_buffer[current_cluster] / FEPS)
			readsectorpio(info->loc + fs_info.fat + current_cluster / FEPS, 1, FAT_buffer, disk);
		if ((FAT_buffer[current_cluster % FEPS] & 0x0FFFFFFF) < 0x0FFFFFF0) {
			++fs_info.root_clusters;
			current_cluster = FAT_buffer[current_cluster % FEPS];
		}
		else break;
	} while (1);

	if (BPB.sector_count_small)
		fs_info.sector_count = BPB.sector_count_small;
	else
		fs_info.sector_count = BPB.sector_count_large;

	info->fs = fat32;
	info->fsdata = malloc(sizeof(fat_fs_info));
	memcpy(info->fsdata, &fs_info, sizeof(fat_fs_info));
}



size_t fat_read(void *ptr, size_t bytes, FILE *fp) {
	unsigned int disk = volumes[fp->volume].disk;
	unsigned int part = volumes[fp->volume].partition;
	LBA partoffset = disktable[disk].parts[part].loc;

	const size_t return_value = bytes;

	fat_fs_info *fs_info = (fat_fs_info *) disktable[disk].parts[part].fsdata;
	const u32 bytes_per_cluster = fs_info->bytes_per_sector * fs_info->sectors_per_cluster;
	u32 skipped_clusters = fp->pointer / bytes_per_cluster;

	u32 *FAT_buffer = malloc(SECTOR_SIZE);
	u8 *buffer = malloc(bytes_per_cluster);
	char filename[256];

	FAT_entry entry;
	u32 active_cluster;

	find_fat_entry(&entry, filename, fp);
	active_cluster = entry.cluster_high << 16 | entry.cluster_low;

	readsectorpio(partoffset + fs_info->fat + active_cluster / FEPS, 1, FAT_buffer, disk);

	for (u32 i = 0; i < skipped_clusters; ++i) {
		if (active_cluster / FEPS != FAT_buffer[active_cluster % FEPS] / FEPS) {
			readsectorpio(partoffset + fs_info->fat + FAT_buffer[active_cluster % FEPS] / FEPS, fs_info->sectors_per_cluster, FAT_buffer, disk);
		}
		active_cluster = FAT_buffer[active_cluster % FEPS];
	}

	if (fp->pointer % bytes_per_cluster) {
		readsectorpio(partoffset + fs_info->data + active_cluster * fs_info->sectors_per_cluster, (fs_info->bytes_per_sector / SECTOR_SIZE) * fs_info->sectors_per_cluster, buffer, disk);

		if (bytes > bytes_per_cluster - fp->pointer % bytes_per_cluster) {
			memcpy(ptr, buffer + fp->pointer % bytes_per_cluster, bytes_per_cluster - fp->pointer % bytes_per_cluster);
			ptr += bytes_per_cluster - fp->pointer % bytes_per_cluster;
			bytes -= bytes_per_cluster - fp->pointer % bytes_per_cluster;
		}
		else {
			memcpy(ptr, buffer + fp->pointer % bytes_per_cluster, bytes);
			return return_value;
		}

		if (active_cluster / FEPS != FAT_buffer[active_cluster % FEPS] / FEPS) {
			readsectorpio(partoffset + fs_info->fat + FAT_buffer[active_cluster % FEPS] / FEPS, fs_info->sectors_per_cluster, FAT_buffer, disk);
		}
		active_cluster = FAT_buffer[active_cluster % FEPS];
	}

	do {
		readsectorpio(partoffset + fs_info->data + active_cluster * fs_info->sectors_per_cluster, (fs_info->bytes_per_sector / SECTOR_SIZE) * fs_info->sectors_per_cluster, buffer, disk);
		if (bytes > bytes_per_cluster) {
			memcpy(ptr, buffer, bytes_per_cluster);
			bytes -= bytes_per_cluster;
		}
		else {
			memcpy(ptr, buffer, bytes);
			break;
		}
		ptr += bytes_per_cluster;

		if (active_cluster / FEPS != FAT_buffer[active_cluster % FEPS] / FEPS) {
			readsectorpio(partoffset + fs_info->fat + FAT_buffer[active_cluster % FEPS] / FEPS, fs_info->sectors_per_cluster, FAT_buffer, disk);
		}
		else
			active_cluster = FAT_buffer[active_cluster % FEPS];

	} while ((active_cluster & 0x0FFFFFFF) < 0x0FFFFFF0);

	free(FAT_buffer);
	return return_value;
}



int find_fat_entry(FAT_entry *entry, char filename[256], const FILE *fp) {
	/* entry and name parameters are both outputs */

	char volume = fp->volume;

	unsigned int disk = volumes[volume].disk;
	unsigned int part = volumes[volume].partition;
	LBA partoffset = disktable[disk].parts[part].loc;
	fat_fs_info *fs_info = (fat_fs_info *) disktable[disk].parts[part].fsdata;

	unsigned int i, j, len = 0;

	char charbuf[6];
	unsigned int depth = 0;

	u32 active_cluster = fs_info->root_cluster;
	u32 *FAT_buffer = malloc(SECTOR_SIZE);
	const u32 bytes_per_cluster = fs_info->bytes_per_sector * fs_info->sectors_per_cluster;
	FAT_entry *buffer = malloc(bytes_per_cluster);

	FAT_entry *current;

	readsectorpio(partoffset + fs_info->fat + active_cluster / FEPS, 1, FAT_buffer, disk);

	do {
inloop:
		readsectorpio(partoffset + fs_info->data + active_cluster * fs_info->sectors_per_cluster, fs_info->sectors_per_cluster, buffer, disk);
inloop2:
		current = buffer;

		/* Under this, everything to right of '<' is the amount of fat entries in a cluster */
		for (i = 0; i < bytes_per_cluster / sizeof(FAT_entry); ++i) {
			if (current->attr == LFN) {
				while (current->attr == LFN) {
					wcatomba((u8*) charbuf, ((LFN_entry*) current)->first, 5);
					memcpy(&filename[(((((LFN_entry*) current)->order) & 0x1F) - 1) * 13], charbuf, 5);

					wcatomba((u8*) charbuf, ((LFN_entry*) current)->middle, 6);
					memcpy(&filename[(((((LFN_entry*) current)->order) & 0x1F) - 1) * 13 + 5], charbuf, 6);

					wcatomba((u8*) charbuf, ((LFN_entry*) current)->last, 2);
					memcpy(&filename[(((((LFN_entry*) current)->order) & 0x1F) - 1) * 13 + 11], charbuf, 2);

					len += 13;
					++current;
					++i;

					if (i >= bytes_per_cluster / sizeof(FAT_entry)) {
						readsectorpio(partoffset + fs_info->fat + FAT_buffer[active_cluster % FEPS] / FEPS, fs_info->sectors_per_cluster, FAT_buffer, disk);
						active_cluster = FAT_buffer[active_cluster % FEPS];
						goto inloop2;
					}
				}
				filename[len] = '\0';
				len = 0;
			}
			else {
				for (j = 0; (filename[j] = current->name[j]) != ' ' && j < 8; ++j);
				filename[j] = '.';
				memcpy(&filename[j + 1], current->extension, 3);
				filename[j + 4] = '\0';
			}

			if (!strcmp(filename, fp->path[depth])) {
				active_cluster =  current->cluster_high << 16 | current->cluster_low;
				memcpy(entry, current, sizeof(FAT_entry));
				if (!(current->attr & 0x10)) return 0;
				++depth;
				goto inloop;
			}
			++current;
		}
		readsectorpio(partoffset + fs_info->fat + FAT_buffer[active_cluster % FEPS] / FEPS, fs_info->sectors_per_cluster, FAT_buffer, disk);
		active_cluster = FAT_buffer[active_cluster % FEPS];
	} while ((active_cluster & 0x0FFFFFFF) < 0x0FFFFFF0);

	return 2;
}



unsigned long long fat_filesize(FILE *fp) {
	FAT_entry entry;
	char filename[256];
	find_fat_entry(&entry, filename, fp);
	return (fp->size = entry.size);
}
