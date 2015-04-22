;
; sc11.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 11 = not used
sys11:
	add	$8,$0,11
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
