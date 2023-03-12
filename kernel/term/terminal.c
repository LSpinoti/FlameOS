#include <stdlib.h>
#include <gfx.h>

extern char _binary_data_font_psfu_start;
extern char _binary_data_font_psfu_end;

typedef struct {
	u32 magic;
	u32 version;
	u32 header_size;
	u32 flags;
	u32 glyphs;
	u32 bytes_per_glyph;
	u32 h;
	u32 w;
} PSFU_header_t;

void putchar(unsigned int c, unsigned int x, unsigned int y, pixel_t fg, pixel_t bg) {
	PSFU_header_t *const hdr = (PSFU_header_t *) &_binary_data_font_psfu_start;

//	u8 *ptr = framebuffer +
}
