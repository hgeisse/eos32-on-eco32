;
; sc31.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 31 = not used
sys31:
	add	$8,$0,31
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
