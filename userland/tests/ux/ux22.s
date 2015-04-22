;
; ux22.s -- user mode exception test program
;

	.set	SYS_EXIT,1
	.set	SYS_FORK,2
	.set	SYS_WAIT,7
	.set	SYS_SYNC,36

	.data

	; user mode exception 22: TLB write
	; --> SIGSEG, core image
	;
	; this test needs support in the kernel
	; (see ureg.c)
umx22:
	add	$8,$0,SYS_FORK
	trap
	bne	$2,$0,umx22a
	stw	$0,$0,0x40000000
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
umx22a:
	add	$8,$0,SYS_WAIT
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
