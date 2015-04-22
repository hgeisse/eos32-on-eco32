;
; sc50.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 50 = not used
sys50:
	add	$8,$0,50
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
