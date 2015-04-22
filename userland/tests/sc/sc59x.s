;
; sc59x.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
