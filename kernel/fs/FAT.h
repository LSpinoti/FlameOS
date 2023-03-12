#ifndef FAT_H
#define FAT_H
#include <types.h>
#include <FS.h>

#define FEPS 128 /* Fat Entries Per Sector (LBA) */

typedef struct {
	/* BPB */
	u8 jumpcode[3];
	char OEM[8];
	u16 bytes_per_sector;
	u8 sectors_per_cluster;
	u16 reserved_sectors;
	u8 FAT_count;
	u16 root_entries;
	u16 sector_count_small;
	u8 type;
	u16 unused;
	u16 sectors_per_track;
	u16 heads;
	u32 LBA;
	u32 sector_count_large;
	/* EBR */
	u32 sectors_per_FAT;
	u16 flags;
	u16 version;
	u32 root_cluster;
	u16 fsinfo_sector;
	u16 unused2;
	u8 reserved[12];
	u8 drivenum;
	u8 unused_flags;
	u8 signature;
	u32 serial;
	char label[11];
	char idstring[8];
	u8 code[420];
	u16 bootsig;
} __attribute__((packed)) bootrecord;


/* fat_fs_info is just a cleaned bootrecord */
typedef struct {
	u32 sector_count;

	u16 bytes_per_sector;
	u8 sectors_per_cluster;
	u32 sectors_per_fat;

	u8 fat_count;

	u16 root_entries;
	u16 root_sectors;
	u32 root_cluster;
	u16 root_clusters;

	/* All locations below are in LBA 512 byte sectors and begin from start of partition */
	u32 fat;
	u32 data;
} fat_fs_info;

typedef struct {
	u32 leadsig;
	u8 reserved[480];
	u32 midsig;
	u32 last_cluster;
	u32 cluster_hint;
	u8 reserved2[12];
	u32 trailsig;
} __attribute__((packed)) fsinfo;

typedef enum {
	READ_ONLY = 0x01,
	HIDDEN = 0x02,
	SYSTEM = 0x04,
	VOLUME_ID = 0x08,
	DIRECTORY = 0x10,
	ARCHIVE = 0x20,
	LFN = 0x0F
} __attribute__((packed)) fat_attr;

typedef struct {
	unsigned int hour : 5;
	unsigned int minutes : 6;
	unsigned int seconds : 5;
} __attribute__((packed)) fat_time;

typedef struct {
	unsigned int year : 7;
	unsigned int month : 4;
	unsigned int day : 5;
} __attribute__((packed)) fat_date;

typedef struct {
	char name[8];
	char extension[3];
	fat_attr attr;
	u8 NT_flags;
	u8 creation_centisecond;
	fat_time creation_time;
	fat_date creation_date;
	fat_date last_accessed;
	u16 cluster_high;
	fat_time mod_time;
	fat_date mod_date;
	u16 cluster_low;
	u32 size; /* Bytes */
} __attribute__((packed)) FAT_entry;

typedef struct {
	u8 order;
	wchar first[5];
	fat_attr attr;
	u8 long_entry;
	u8 checksum;
	wchar middle[6];
	u16 zero;
	wchar last[2];
} __attribute__((packed)) LFN_entry;

void fat_setup(unsigned int disk, unsigned int part);
size_t fat_read(void *ptr, size_t bytes, FILE *fp);
int find_fat_entry(FAT_entry *entry, char filename[256], const FILE *fp);
unsigned long long fat_filesize(FILE *fp);

#endif
