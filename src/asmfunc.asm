.syntax unified

.section .text.asm_function,"ax",%progbits
.global _strlen
_strlen:
	add	r2,r0,#1
_stl0:	ldrb	r1,[r0],#1
	orrs	r1,r1
	bne	_stl0
	sub	r0,r0,r2
	bx	lr

.global _stpcpy
_stpcpy:
_stp0:	ldrb	r2,[r1],#1
	cbz	r2,_stp1
	strb	r2,[r0],#1
	b	_stp0
_stp1:	bx	lr

.global __get_MSP
__get_MSP:
	mrs	r0,msp
	bx	lr 

.global __get_PSP
__get_PSP:
	mrs	r0,psp
	bx	lr 

.global __get_CONTROL
__get_CONTROL:
	mrs	r0,control
	bx	lr

.global __set_PSP
__set_PSP:
	msr	psp,r0
	bx	lr

.global __set_CONTROL
__set_CONTROL:
	msr	control,r0
	bx	lr
