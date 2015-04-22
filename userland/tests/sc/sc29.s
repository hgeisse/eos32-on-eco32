;
; sc29.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_ALARM,27
	.set	SYS_PAUSE,29
	.set	SYS_SIGNAL,48

	.data

	; 29 = pause
sys29:
	add	$8,$0,SYS_SIGNAL
	add	$4,$0,14		; SIGALRM
	add	$5,$0,sys29fun		; handler
	add	$6,$0,0			; handler return (not used)
	trap
	add	$8,$0,SYS_ALARM
	add	$4,$0,5			; 5 seconds
	trap
	add	$8,$0,SYS_PAUSE
	trap				; should not return
	add	$8,$0,SYS_EXIT
	add	$4,$0,0x99
	trap

sys29fun:
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
