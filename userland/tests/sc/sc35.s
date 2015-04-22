;
; sc35.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_FTIME,35

	.data

	; 35 = ftime
sys35:
	add	$8,$0,SYS_FTIME
	add	$4,$0,timbuf35
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

	.align	4
timbuf35:
	.space	1*4+3*2+2		; note: 2 bytes padding
