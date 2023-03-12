#include <stdio.h>
#include <stdlib.h>
#include <gfx.h>

int tga_plot(FILE *fp, unsigned int x, unsigned int y) {
	tga_info_t *header = malloc(fsize(fp));
	pixel_t *image = (void*) header + sizeof(tga_info_t);

	fread(header, fsize(fp), fp);

	unsigned int i, j;
	unsigned short w, h;
	w = header->w;
	h = header->h;

	for (i = 0; i < h; ++i) {
		for (j = 0; j < w; ++j) {
			plotpixel(j, i, *image++);
		}
	}

	free(header);
	return 0;
}
