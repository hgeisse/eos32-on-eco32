;
; sc46.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_SETGID,46
	.set	SYS_GETGID,47

	.data

	; 46 = setgid
sys46:
	add	$8,$0,SYS_SETGID
	add	$4,$0,0x4321
	trap
	add	$8,$0,SYS_GETGID
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
