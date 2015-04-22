;
; sc62.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 62 = not used
sys62:
	add	$8,$0,62
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
