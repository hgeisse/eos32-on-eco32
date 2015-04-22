;
; sc60.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_UMASK,60

	.data

	; 60 = umask
sys60:
	add	$8,$0,SYS_UMASK
	add	$4,$0,0133
	trap
	add	$8,$0,SYS_UMASK
	add	$4,$0,0000
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
