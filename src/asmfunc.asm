.syntax unified

.section .text.asm_function,"ax",%progbits
.global __strlen
__strlen:
	add	r2,r0,#1
_stl0:	ldrb	r1,[r0],#1
	orrs	r1,r1
	bne	_stl0
	sub	r0,r0,r2
	bx	lr

.global __stpcpy
__stpcpy:
_stp0:	ldrb	r2,[r1],#1
	cbz	r2,_stp1
	strb	r2,[r0],#1
	b	_stp0
_stp1:	bx	lr
