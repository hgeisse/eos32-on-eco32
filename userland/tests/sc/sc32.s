;
; sc32.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 32 = not used
sys32:
	add	$8,$0,32
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
