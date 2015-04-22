;
; ux17.s -- user mode exception test program
;

	.set	SYS_EXIT,1
	.set	SYS_FORK,2
	.set	SYS_WAIT,7
	.set	SYS_SYNC,36

	.data

	; user mode exception 17: illegal instruction
	; --> SIGINS, core image
	;
	; this test needs no support in the kernel
umx17:
	add	$8,$0,SYS_FORK
	trap
	bne	$2,$0,umx17a
	.word	0xFC000000
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
umx17a:
	add	$8,$0,SYS_WAIT
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
