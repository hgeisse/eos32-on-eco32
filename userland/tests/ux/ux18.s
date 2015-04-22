;
; ux18.s -- user mode exception test program
;

	.set	SYS_EXIT,1
	.set	SYS_FORK,2
	.set	SYS_WAIT,7
	.set	SYS_SYNC,36

	.data

	; user mode exception 18: privileged instruction
	; --> SIGINS, core image
	;
	; this test needs no support in the kernel
umx18:
	add	$8,$0,SYS_FORK
	trap
	bne	$2,$0,umx18a
	mvts	$0,0
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
umx18a:
	add	$8,$0,SYS_WAIT
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
