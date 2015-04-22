;
; ux24.s -- user mode exception test program
;

	.set	SYS_EXIT,1
	.set	SYS_FORK,2
	.set	SYS_WAIT,7
	.set	SYS_SYNC,36

	.data

	; user mode exception 24: illegal address
	; --> SIGBUS, core image
	;
	; this test needs no support in the kernel
umx24:
	add	$8,$0,SYS_FORK
	trap
	bne	$2,$0,umx24a
	ldw	$8,$0,0x00000001
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
umx24a:
	add	$8,$0,SYS_WAIT
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
