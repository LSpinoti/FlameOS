/*
 * Warning: 99% of this stuff doesn't work lol
 * I just made it to use the few funtions I need :]
*/

#ifndef UEFI_H
#define UEFI_H

#include "boot.h"

typedef enum {
	coldreset,
	warmreset,
	shutdown,
	platformreset
} RESET;

typedef struct {
	u32 part1;
	u16 part2;
	u16 part3;
	u8 part4[8];
} guid;

typedef struct {
	u64 sig;
	u32 ver;
	u32 size;
	u32 crc32;
	u32 res;
} tab_header;

typedef struct {} *handle;

typedef struct {
	tab_header hdr;
	void (*gettime)();
	void (*settime)();
	void (*getwaketime)();
	void (*setwaketime)();
	u64 (*setvirtaddrmap)(size_t MemoryMapSize, size_t DescriptorSize, u32 DescriptorVersion, memmap_t *VirtualMap);
	void (*convptr)();
	void (*getvar)();
	void (*getnextvar)();
	void (*setvar)();
	void (*getnextcount)();
	void (*reset)(RESET, u8, u8, void *);
	void (*updatecapsule)();
	void (*querycapsule)();
	void (*queryvarinfo)();
} rs;

#define GOPGUID {0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}}

typedef enum {
	any,
	max,
	addr,
	none
} ALLOC_TYPE;

typedef enum {
	resmem,
	loadermem,
	loaderdatamem,
	bscodemem,
	bsdatamem,
	rscodemem,
	rsdatamem,
	normalmem,
	badmem,
	acpimem,
	fwmem,
	iomem,
	ioportmem,
	palmem,
	persistmem,
	nonemem
} MEM_TYPE;

typedef struct {
	tab_header hdr;
	void (*raiseTPL)();
	void (*restoreTPL)();
	void* (*pagealloc)(ALLOC_TYPE type, MEM_TYPE memtype, u64 pages, void *addr);
	u64 (*pagefree)(void *addr, u64 pages);
	u64 (*getmemmap)(u64 *size, void* memmap, u64 *mapkey, u64 *descsize, u32 *descver);
	void (*poolalloc)();
	void (*poolfree)();
	void (*createevent)();
	void (*settimer)();
	void (*waitforevent)();
	void (*signalevent)();
	void (*closeevent)();
	void (*checkevent)();
	void (*installprotocol)();
	void (*reinstallprotocol)();
	void (*uninstallprotocol)();
	void (*handleprotocol)();
	void (*reserved);
	void (*registerprotocol)();
	void (*lochandle)();
	void (*locdevice)();
	void (*installcfgtable)();
	void (*loadimg)();
	void (*startimg)();
	void (*exit)();
	void (*unloadimg)();
	void (*exitbs)(handle imagehandle, u64 mapkey);
	void (*getnextmonocount)();
	void (*stall)(u64 ms);
	void (*setwatchdog)();
	void (*connectctrl)();
	void (*disconnectctrl)();
	void (*openprotocol)();
	void (*closeprotocol)();
	void (*openprotocolinfo)();
	void (*protocolsperhndl)();
	void (*lochandlebuf)();
	u64 (*locprotocol)(guid* guid, void *reg, void **iface);
	void (*instprotocol)();
	void (*unistprotocol)();
	void (*calccrc32)();
	void (*memcpy)();
	void (*memset)();
	void (*createeventex)();
} bs;

enum PIXFMT {
	rgb32bpp,
	bgr32bpp,
	bitmask,
	bltonly,
	fmtmax
};

typedef struct {
	u32 r;
	u32 g;
	u32 b;
	u32 res;
} pixinfo;

typedef struct {
	u32 ver;
	u32 w;
	u32 h;
	enum PIXFMT pixelfmt;
	pixinfo pixinfo;
	u32 pixperline;
} gopmodeinfo;

typedef struct {
	u32 maxmode;
	u32 mode;
	gopmodeinfo *info;
	u64 sizeofinfo;
	void *fb;
	u64 fbsize;
} gopmode;

typedef struct {
	void (*querymode)(void *gop, u64 modenum, u64 *infosize, gopmodeinfo **info);
	void (*setmode)(void *gop, u32 modenum);
	void (*blt)();
	gopmode *mode;
} gop;

typedef struct {
	tab_header hdr;
	wchar* vendorstr;
	u32 fwver;
	handle handle1;
	void *conin;
	handle handle2;
	void *conout;
	handle handle3;
	void *stderr;
	rs* rs;
	bs* bs;
	u64 cfgtabs;
	void *cfgtab;
} systable;

#endif
