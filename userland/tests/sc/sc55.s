;
; sc55.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 55 = not used
sys55:
	add	$8,$0,55
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
