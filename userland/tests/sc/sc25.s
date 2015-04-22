;
; sc25.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_STIME,25

	.data

	; 25 = stime
sys25:
	add	$8,$0,SYS_STIME
	add	$4,$0,0x12345678
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
