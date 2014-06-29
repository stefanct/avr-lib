;************************************************************************
;*                                                                      *
;*                 AVR-GCC: Much better 64 bit support                  *
;*         (Speeds up multiplication, division and modulo only)         *
;*                       Author: Peter Dannegger                        *
;*                                                                      *
;************************************************************************
;r18-r27, r30, r31: scratch pad register
;r2-r17, r28, r29: must be saved
;r0, r1: not usable

#define	t0	r4
#define	t1	r5
#define	t2	r6
#define	t3	r7
#define	t4	r8
#define	t5	r9
#define	t6	r26
#define	t7	r27
#define	cnt	r31

#define	a0	r18
#define	a1	r19
#define	a2	r20
#define	a3	r21
#define	a4	r22
#define	a5	r23
#define	a6	r24
#define	a7	r25

#define	b0	r10
#define	b1	r11
#define	b2	r12
#define	b3	r13
#define	b4	r14
#define	b5	r15
#define	b6	r16
#define	b7	r17


; r25..r18 % r17..r10 = r25..r18
.global __umoddi3
__umoddi3:
	set
	rjmp    __udivdi3_1

; r25..r18 / r17..r10 = r25..r18
.global __udivdi3
__udivdi3:
	clt
__udivdi3_1:
	push    t0
	push    t1
	push    t2
	push    t3
	push    t4
	push    t5
	clr     t0
	clr     t1
	movw    t2, t0
	movw    t4, t0
	movw    t6, t0
	ldi     cnt, 64
__udivdi3_2:
	lsl     a0
	rol     a1
	rol     a2
	rol     a3
	rol     a4
	rol     a5
	rol     a6
	rol     a7
	rol     t0
	rol     t1
	rol     t2
	rol     t3
	rol     t4
	rol     t5
	rol     t6
	rol     t7
	cp      t0, b0
	cpc     t1, b1
	cpc     t2, b2
	cpc     t3, b3
	cpc     t4, b4
	cpc     t5, b5
	cpc     t6, b6
	cpc     t7, b7
	brcs    __udivdi3_3
	sub     t0, b0
	sbc     t1, b1
	sbc     t2, b2
	sbc     t3, b3
	sbc     t4, b4
	sbc     t5, b5
	sbc     t6, b6
	sbc     t7, b7
	inc     a0
__udivdi3_3:
	dec     cnt
	brne    __udivdi3_2
	brtc    restore_regs
	movw    a0, t0
	movw    a2, t2
	movw    a4, t4
	movw    a6, t6
restore_regs:
	pop     t5
	pop     t4
	pop     t3
	pop     t2
	pop     t1
	pop     t0
	ret

; r25..r18 * r17..r10 = r25..r18
.global __muldi3
__muldi3:
	push    t0
	push    t1
	push    t2
	push    t3
	push    t4
	push    t5
	clr     t0
	clr     t1
	movw    t2, t0
	movw    t4, t0
	movw    t6, t0
	ldi     cnt, 65
	rjmp    __muldi3_3
__muldi3_1:
	brcc    __muldi3_2
	add     t0, b0
	adc     t1, b1
	adc     t2, b2
	adc     t3, b3
	adc     t4, b4
	adc     t5, b5
	adc     t6, b6
	adc     t7, b7
__muldi3_2:
	ror     t7
	ror     t6
	ror     t5
	ror     t4
	ror     t3
	ror     t2
	ror     t1
	ror     t0
__muldi3_3:
	ror     a7
	ror     a6
	ror     a5
	ror     a4
	ror     a3
	ror     a2
	ror     a1
	ror     a0
	dec     cnt
	brne    __muldi3_1
	rjmp    restore_regs
