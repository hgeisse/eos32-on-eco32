;
; sc43.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_TIMES,43

	.data

	; 43 = times
sys43:
	ldhi	$8,0x00400000
sys43a:
	sub	$8,$8,1
	bne	$8,$0,sys43a
	add	$8,$0,SYS_TIMES
	add	$4,$0,tbuf43
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

	.align	4
tbuf43:
	.space	4*4
