#include <stdarg.h>
#include <stdlib.h>
#include <cpu/IO.h>
#define COM1 0x3F8

unsigned int logs(char *str) {
	unsigned int count = 0;
	while (*str) {
		outb(COM1, *str);
		++count;
		if (*str == '\n') outb(COM1, '\r');
		++str;
	}
	return count;
}

void logchar(char c) {
	outb(COM1, c);
	if (c == '\n') outb(COM1, '\r');
}

void debug_log(char *format, ...) {
	va_list ap;
	char str[21];
	va_start(ap, format);
	while (*format) {
		if (*format == '%') {
			switch (*(format + 1)) {
			case 'p':
				itoa(va_arg(ap, long long), str, 16);
				logs("0x");
				logs(str);
				++format;
				break;
			case 'c':
				logchar(va_arg(ap, int));
				++format;
				break;
			case 'd':
				itoa(va_arg(ap, long long), str, 10);
				logs(str);
				++format;
				break;
			case 'X':
				itoa(va_arg(ap, long long), str, 16);
				logs(str);
				++format;
				break;
			case 's':
				logs(va_arg(ap, char*));
				++format;
				break;
			case '%':
				outb(COM1, '%');
				break;
			}
			++format;
		}
		else {
			outb(COM1, *format);
			if (*format == '\n') outb(COM1, '\r');
			++format;
		}
	}
	va_end(ap);
}

__attribute__((no_caller_saved_registers)) void error(char *format, ...) {
	/* Same thing as log, just with "no_caller_saved_registers"
	 * If anyone knows how to dump all of the ... parameters from this function
	 * into the log function, message me :) */
	va_list ap;
	char str[21];
	va_start(ap, format);
	while (*format) {
		if (*format == '%') {
			switch (*(format + 1)) {
			case 'p':
				itoa(va_arg(ap, long long), str, 16);
				logs("0x");
				logs(str);
				++format;
				break;
			case 'd':
				itoa(va_arg(ap, long long), str, 10);
				logs(str);
				++format;
				break;
			case 'X':
				itoa(va_arg(ap, long long), str, 16);
				logs(str);
				++format;
				break;
			case 's':
				logs(va_arg(ap, char*));
				++format;
				break;
			case '%':
				outb(COM1, '%');
				break;
			}
			++format;
		}
		else {
			outb(COM1, *format);
			++format;
		}
	}
	va_end(ap);
}

void memdump(const void *ptr, size_t count) {
	unsigned char *c = (unsigned char*) ptr;
	unsigned int x;

	for (;;) {
		for (x = 0; x < 8; ++x) {
			if (!count) return;
			if (*c < 0x10) {
				logchar('0');
			}
			debug_log("%X ", *c++);

			--count;
		}
		logs("\n\r");
	}
}
