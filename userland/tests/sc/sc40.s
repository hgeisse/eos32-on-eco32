;
; sc40.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 40 = not used
sys40:
	add	$8,$0,40
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
