;
; sc23.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_SETUID,23
	.set	SYS_GETUID,24

	.data

	; 23 = setuid
sys23:
	add	$8,$0,SYS_SETUID
	add	$4,$0,0x1234
	trap
	add	$8,$0,SYS_GETUID
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
