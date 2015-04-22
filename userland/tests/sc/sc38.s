;
; sc38.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 38 = not used
sys38:
	add	$8,$0,38
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
