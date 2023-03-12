#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <syscall.h>
#include <FS.h>

FILE *fopen(char *path, u8 flags) {
	void *oldpath = path; // Don't change this variable
	unsigned int i, j = 0;
	FILE *fp = files;

	// Count '/'s in path to see how many elements in array I will use
	unsigned int dirs;
	for (dirs = 0; *path; ++path) if (*path == '/') ++dirs;
	if (dirs == 0) return 0;
	path = oldpath;

	char **fpath = malloc((dirs + 1) * sizeof(char*)); // Keep 1 extra for the '\0' symbol signifying end of path

	for (i = 0; i < dirs; ++i)
		fpath[i] = malloc(FILENAME_LENGTH);
	fpath[i + 1] = NULL;

	if (*path++ != '|') goto error;
	char volume = *path++ - 'A';
	if (volume >= 26) goto error;
	// TODO: allow over 26 volumes
	if (*path++ != '/') goto error;

	while (1) {
		i = 0;
		while (*path != '/') {
			if (*path == '\0') {
				fpath[j + 1] = NULL;
				goto main_1;
			}
			fpath[j][i] = *path;
			++i;
			++path;
		}
		fpath[j][i] = '\0';
		++j;
		++path;
	}
main_1:

	while (fp->used) ++fp;
	fp->used = TRUE;
	fp->path = fpath;
	fp->pointer = 0;
	fp->volume = volume;

	switch (disktable[volumes[volume].disk].parts[volumes[volume].partition].fs) {
		case fat32:
			fp->size = fat_filesize(fp);
			break;
		default: return 0;
	}

	return fp;

error:
	free(fpath);
	return 0;
}

int aligncheck(FILE *fp) {
	// Boundary check
	if (fp < files || fp >= files + MAX_OPEN_FILES) {
		return 1;
	}
	// Lined-up check
	if ((unsigned long long) fp % sizeof(FILE) != (unsigned long long) files % sizeof(FILE)) {
		return 2;
	}

	return 0;
}

int fclose(FILE *fp) {
	if (aligncheck(fp)) return 1;
	fp->used = 0;

	char **ptr = fp->path;

	while (*ptr != NULL) free(*ptr++);
	free(fp->path);

	return 0;
}

size_t fread(void *ptr, size_t bytes, FILE *fp) {
	if (bytes == 0) return 0;
	if (bytes > fp->size - fp->pointer) return 0;

	unsigned int disk = volumes[fp->volume].disk;
	unsigned int part = volumes[fp->volume].partition;

	switch (disktable[disk].parts[part].fs) {
		case fat32:
			return fat_read(ptr, bytes, fp);
			break;
		default:
			return 0;
			break;
	}
	return 0;
}

int fseek(FILE *fp, long long offset, fpos_t position) {
	switch (position) {
		case SEEK_SET:
			if (offset >= fp->size) return -2;
			if (offset < 0) return -2;
			fp->pointer = offset;
			break;
		case SEEK_CUR:
			if (offset + fp->pointer >= fp->size) return -2;
			if (offset + fp->pointer < 0) return -2;
			fp->pointer += offset;
			break;
		case SEEK_END:
			if (offset >= 0) return -2;
			if (-offset > fp->size) return -2;
			fp->pointer = fp->size + offset;
		default: return -1;
	}
	return 0;
}

void rewind(FILE *fp) {
	if (aligncheck(fp)) return;
	fp->pointer = 0;
}

unsigned long long fsize(FILE *fp) {
	if (aligncheck(fp)) return 0;
	return fp->size;
}
