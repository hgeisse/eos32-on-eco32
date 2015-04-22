;
; sc00.s -- system call test program
;

	.set	SYS_GETADR,0
	.set	SYS_EXIT,1

	.data

	; 0 = getadr
sys00:
	add	$8,$0,SYS_GETADR
	add	$4,$0,0
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
