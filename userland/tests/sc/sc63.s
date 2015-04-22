;
; sc63.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 63 = not used
sys63:
	add	$8,$0,63
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
