;
; sc34.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_NICE,34

	.data

	; 34 = nice
sys34:
	add	$8,$0,SYS_NICE
	add	$4,$0,10
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
