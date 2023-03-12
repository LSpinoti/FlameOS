#include <cpu/IO.h>

#define EFER	0xC0000080
#define STAR	0xC0000081
#define LSTAR	0xC0000082
#define SFMASK	0xC0000084

void sys_handler() {
	u64 code;

	u64 arg1;
	u64 arg2;
	u64 arg3;
	u64 arg4;
	u64 arg5;
	u64 arg6;

	void *return_addr;

	asm (
	"cli\n"
	"mov %%rax, %0\n"
	"mov %%rcx, %1\n"
	"mov %%rdi, %2\n"
	"mov %%rsi, %3\n"
	"mov %%rdx, %4\n"
	"mov %%r8,  %5\n"
	"mov %%r9,  %6\n"
	"mov %%r10, %7\n"
	: "=m"(code), "=m"(return_addr), "=m"(arg1), "=m"(arg2), "=m"(arg3), "=m"(arg4), "=m"(arg5), "=m"(arg6));

	switch (code) {
	}

	asm(
	"mov %0,   %%rcx\n"
	"mov $0x2, %%r11\n"
	"sysretq\n"
	:: "r"(return_addr));
}

void init_syscall() {
	wrmsr(EFER, rdmsr(EFER) | 1);
	wrmsr(SFMASK, 0);
	wrmsr(STAR, 0x0018000800000000);
	wrmsr(LSTAR, (u64) sys_handler);
}

void enter_userspace(void *RIP, void *RSP) {
	asm (
	"mov %0, %%rcx\n"
	"mov %1, %%rax\n"
	"mov %%rax, %%rsp\n"
	"mov $0x202, %%r11\n"
	"sysretq\n"
	:: "a"(RSP), "c"(RIP));
}
