#include <gfx.h>

void plotpixel(unsigned int x, unsigned int y, pixel_t colour) {
	pixel_t *loc = fb_addr + x + y * fb_pixperline;
	*loc = colour;
}
