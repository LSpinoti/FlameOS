#ifndef TYPES_H
#define TYPES_H

typedef enum {
	FALSE,
	TRUE
} bool;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long s64;

typedef unsigned long size_t;

typedef signed short wchar;

#define NULL ((void*)0)

#endif
