;
; setjmp.s -- non-local jumps
;


	.export	setjmp
	.export	longjmp


	; int setjmp(jmp_buf env)
setjmp:
	stw	$16,$4,0*4		; save local variables
	stw	$17,$4,1*4
	stw	$18,$4,2*4
	stw	$19,$4,3*4
	stw	$20,$4,4*4
	stw	$21,$4,5*4
	stw	$22,$4,6*4
	stw	$23,$4,7*4
	stw	$29,$4,8*4		; save sp
	stw	$31,$4,9*4		; save pc
	add	$2,$0,0			; return 0
	jr	$31


	; void longjmp(jmp_buf env, int val)
longjmp:
	ldw	$16,$4,0*4		; restore local variables
	ldw	$17,$4,1*4
	ldw	$18,$4,2*4
	ldw	$19,$4,3*4
	ldw	$20,$4,4*4
	ldw	$21,$4,5*4
	ldw	$22,$4,6*4
	ldw	$23,$4,7*4
	ldw	$29,$4,8*4		; restore sp
	ldw	$31,$4,9*4		; restore pc
	add	$2,$5,0			; return val
	bne	$2,$0,longjmp1
	add	$2,$0,1			; but replace 0 by 1
longjmp1:
	jr	$31
