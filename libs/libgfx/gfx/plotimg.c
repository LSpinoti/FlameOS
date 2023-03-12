#include <gfx.h>

void plotimg(const pixel_t *src, unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
	unsigned int i, j;

	const pixel_t *p = src;

	pixel_t *ptr = (pixel_t *) fb_addr + y * fb_pixperline + x;

	for (i = 0; i < h; ++i) {
		for (j = 0; j < w; ++j) {
			*ptr++ = *src++;
		}
		ptr += fb_w - w;
	}
}
