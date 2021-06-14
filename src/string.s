.syntax unified

.section .text.memcpy,"ax",%progbits
.global memcpy
memcpy:
	cbz	r2,_mcl1
_mcl0:	ldrb	r3,[r0],#1
	strb	r3,[r1],#1
	subs	r2,r2,#1
	bne	_mcl0
_mcl1:	bx	lr

.section .text.memset,"ax",%progbits
.global memset
memset:	
	cbz	r2,_msl1
_msl0:	strb	r1,[r0],#1
	subs	r2,r2,#1
	bne	_msl0
_msl1:	bx	lr

.section .text.strlen,"ax",%progbits
.global strlen
strlen:
	add	r2,r0,#1
_len0:	ldrb	r1,[r0],#1
	orrs	r1,r1
	bne	_len0
	sub	r0,r0,r2
	bx	lr

.section .text.stpcpy,"ax",%progbits
.global stpcpy
stpcpy:
_cpy0:	ldrb	r2,[r1],#1
	cbz	r2,_cpy1
	strb	r2,[r0],#1
	b	_cpy0
_cpy1:	bx	lr

.section .text.strchr,"ax",%progbits
.global strchr
strchr:
_chr0:	ldrb	r2,[r0],#1
	cbz	r2,_chr1
	subs	r2,r1
	bne	_chr0
	sub	r0,#1
	bx	lr
_chr1:	subs	r0,r0
	bx	lr

.section .text.strcmp,"ax",%progbits
.global strcmp
strcmp:
_cmp0:	ldrb	r2,[r0],#1
	ldrb	r3,[r1],#1
	cbnz	r2,_cmp1
	cbnz	r3,_cmp1
	subs	r0,r0
	bx	lr
_cmp1:	subs	r2,r3
	beq	_cmp0
	mov	r0,r2
	bx	lr
