;
; sc07.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_FORK,2
	.set	SYS_WAIT,7

	.data

	; 7 = wait
sys07:
	add	$8,$0,SYS_FORK
	trap
	bne	$2,$0,sys07a
	add	$8,$0,SYS_EXIT
	add	$4,$0,0x42
	trap
sys07a:
	add	$8,$0,SYS_WAIT
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
