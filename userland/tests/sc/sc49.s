;
; sc49.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_SIGNAL,48
	.set	SYS_SIGRET,49

	.data

	; 49 = sigret
sys49:
	add	$8,$0,SYS_SIGNAL
	add	$4,$0,8			; SIGFPE
	add	$5,$0,sys49fun		; handler
	add	$6,$0,sys49ret		; handler return
	trap
	add	$8,$0,0x42
	div	$8,$8,0
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

sys49fun:
	jr	$31

sys49ret:
	add	$8,$0,SYS_SIGRET
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0x42
	trap
