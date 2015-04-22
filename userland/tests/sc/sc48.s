;
; sc48.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_SIGNAL,48

	.data

	; 48 = signal
sys48:
	add	$8,$0,SYS_SIGNAL
	add	$4,$0,8			; SIGFPE
	add	$5,$0,sys48fun		; handler
	add	$6,$0,sys48ret		; handler return
	trap
	add	$8,$0,0x42
	div	$8,$8,0
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

sys48fun:
	jr	$31

sys48ret:
	add	$8,$0,SYS_EXIT
	add	$4,$0,0x42
	trap
