;
; sc37.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_FORK,2
	.set	SYS_WAIT,7
	.set	SYS_KILL,37

	.data

	; 37 = kill
sys37:
	add	$8,$0,SYS_FORK
	trap
	bne	$2,$0,sys37b
sys37a:
	j	sys37a
sys37b:
	add	$8,$0,SYS_KILL
	add	$4,$0,$2		; pid = child
	add	$5,$0,9			; SIGKILL
	trap
	add	$8,$0,SYS_WAIT
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
