;
; sc45.s -- system call test program
;

	.set	SYS_EXIT,1

	.data

	; 45 = not used
sys45:
	add	$8,$0,45
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
