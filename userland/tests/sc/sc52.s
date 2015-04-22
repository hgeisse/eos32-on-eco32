;
; sc52.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 52 = not used
sys52:
	add	$8,$0,52
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
