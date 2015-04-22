;
; sc53.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 53 = not used
sys53:
	add	$8,$0,53
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
