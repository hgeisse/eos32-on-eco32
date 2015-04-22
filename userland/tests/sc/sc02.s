;
; sc02.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_FORK,2
	.set	SYS_WAIT,7

	.data

	; 2 = fork
sys02:
	add	$8,$0,SYS_FORK
	trap
	bne	$2,$0,sys02a
	add	$8,$0,SYS_EXIT
	add	$4,$0,0x42
	trap
sys02a:
	add	$8,$0,SYS_WAIT
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
