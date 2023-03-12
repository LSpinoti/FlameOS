#ifndef GFX_H
#define GFX_H
#include <types.h>
#include <stdio.h>

typedef unsigned int pixel_t;

extern pixel_t* fb_addr;
extern u64 fb_size;
extern u32 fb_w;
extern u32 fb_h;
extern u32 fb_pixperline;

typedef struct {
	u8 magic;
	u8 colormap;
	u8 encoding;
	u16 cmaporig;
	u16 cmaplen;
	u8 cmapent;
	u16 x;
	u16 y;
	u16 h;
	u16 w;
	u8 bpp;
	u8 pixtype;
} __attribute__((packed)) tga_info_t;

void plotimg(const pixel_t *src, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
void plotpixel(unsigned int x, unsigned int y, pixel_t colour);
void fillrect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, pixel_t colour);

int tga_plot(FILE *fp, unsigned int x, unsigned int y);

#endif
