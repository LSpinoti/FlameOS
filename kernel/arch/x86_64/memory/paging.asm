global load_pml4

section .text
load_pml4:
	mov rax, 0x000ffffffffff000
	and rdi, rax
	mov cr3, rdi
	ret
