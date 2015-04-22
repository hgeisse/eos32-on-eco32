;
; sc17.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_BREAK,17

	.data

	; 17 = break
sys17:
	add	$8,$0,SYS_BREAK
	add	$4,$0,0x00018000
	trap
	add	$8,$0,SYS_BREAK
	add	$4,$0,0x00001234
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
