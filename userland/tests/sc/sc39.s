;
; sc39.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 39 = not used
sys39:
	add	$8,$0,39
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
