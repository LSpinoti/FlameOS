#include <gfx.h>
void sleep(unsigned long ticks);
void fillrect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, pixel_t colour) {
	pixel_t *loc = fb_addr + x + y * fb_pixperline;
	int i, j;
	for (i = 0; i < h; ++i) {
		for (j = 0; j < w; ++j) {
			loc[j] = colour;
		}
		loc += fb_pixperline;
	}
}
