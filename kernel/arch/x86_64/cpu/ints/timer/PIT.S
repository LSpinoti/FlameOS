.global pitirq
.extern timerticks

.text
pitirq:
;	pushq %rax

;	movabsq timerticks, %rax
;	inc %rax
;	movabsq %rax, (timerticks)

;	movb $32, %al
;	outb %al, $32

;	popq %rax
	iretq
