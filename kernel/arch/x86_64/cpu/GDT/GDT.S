.global flushregs

.text
flushregs:
	pushq $0x8
	movabsq $flush, %rax
	pushq %rax
	retfq
flush:
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %gs
	movw %ax, %fs
	movw %ax, %ss
	retq
