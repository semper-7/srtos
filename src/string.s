.syntax unified

.section .text.memcpy,"ax",%progbits
.global memcpy
memcpy:
	cbz	r2,mcpy__
mcpy_:	ldrb	r3,[r0],#1
	strb	r3,[r1],#1
	subs	r2,r2,#1
	bne	mcpy_
mcpy__:	bx	lr

.section .text.memset,"ax",%progbits
.global memset
memset:	
	cbz	r2,mset__
mset_:	strb	r1,[r0],#1
	subs	r2,r2,#1
	bne	mset_
mset__:	bx	lr

.section .text.strlen,"ax",%progbits
.global strlen
strlen:
	add	r2,r0,#1
rlen_:	ldrb	r1,[r0],#1
	orrs	r1,r1
	bne	rlen_
	sub	r0,r0,r2
	bx	lr

.section .text.stpcpy,"ax",%progbits
.global stpcpy
stpcpy:	ldrb	r2,[r1],#1
	cbz	r2,pcpy_
	strb	r2,[r0],#1
	b	stpcpy
pcpy_:	bx	lr

.section .text.strcpy,"ax",%progbits
.global strcpy
strcpy: mov	r3,r0
rcpy_:	ldrb	r2,[r1],#1
	cbz	r2,rcpy__
	strb	r2,[r3],#1
	b	rcpy_
rcpy__:	bx	lr

.section .text.strchr,"ax",%progbits
.global strchr
strchr:	ldrb	r2,[r0],#1
	cbz	r2,rchr_
	subs	r2,r1
	bne	strchr
	sub	r0,#1
	bx	lr
rchr_:	subs	r0,r0
	bx	lr

.section .text.strcmp,"ax",%progbits
.global strcmp
strcmp:	ldrb	r2,[r0],#1
	ldrb	r3,[r1],#1
	cbnz	r2,rcmp_
	cbnz	r3,rcmp_
	subs	r0,r0
	bx	lr
rcmp_:	subs	r2,r3
	beq	strcmp
	mov	r0,r2
	bx	lr

.section .text.ecmp,"ax",%progbits
.global ecmp
ecmp:	ldrb	r2,[r0],#1
	ldrb	r3,[r1],#1
	cbnz	r3,ecmp_
	subs	r0,r0
	bx	lr
ecmp_:	subs	r2,r3
	beq	ecmp
	mov	r0,r2
	bx	lr

.section .text.atou,"ax",%progbits
.global atou
atou:	subs	r2,r2
atou_:	ldrb	r1,[r0],#1
	cmp	r1,#48
	bcc	atou__
	cmp	r1,#59
	bcs	atou__
	and	r1,#15
	add	r2,r2,r2,lsl #2
	add	r2,r1,r2,lsl #1
	b	atou_
atou__:	mov	r0,r2
	bx	lr

.section .text.atoip,"ax",%progbits
.global atoip
atoip:	subs	r2,r2
atoi_:	ldrb	r3,[r0],#1
	cmp	r3,#48
	bcc	atoi__
	cmp	r3,#59
	bcs	atoi__
	and	r3,#15
	add	r2,r2,r2,lsl #2
	add	r2,r3,r2,lsl #1
	b	atoi_
atoi__:	strb	r2,[r1],#1
	cbz	r3,atoip_
	cmp	r3,#20
	bne	atoip
atoip_: bx	lr
