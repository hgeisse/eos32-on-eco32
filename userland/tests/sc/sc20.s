;
; sc20.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_GETPID,20

	.data

	; 20 = getpid
sys20:
	add	$8,$0,SYS_GETPID
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
