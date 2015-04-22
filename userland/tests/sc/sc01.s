;
; sc01.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 1 = exit
sys01:
	add	$8,$0,SYS_EXIT
	add	$4,$0,42
	trap
