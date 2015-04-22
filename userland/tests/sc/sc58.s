;
; sc58.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 58 = not used
sys58:
	add	$8,$0,58
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
