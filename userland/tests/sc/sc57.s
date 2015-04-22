;
; sc57.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 57 = not used
sys57:
	add	$8,$0,57
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
