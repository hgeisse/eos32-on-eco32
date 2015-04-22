;
; sc27.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_ALARM,27
	.set	SYS_SIGNAL,48

	.data

	; 27 = alarm
sys27:
	add	$8,$0,SYS_SIGNAL
	add	$4,$0,14		; SIGALRM
	add	$5,$0,sys27fun		; handler
	add	$6,$0,0			; handler return (not used)
	trap
	add	$8,$0,SYS_ALARM
	add	$4,$0,5			; 5 seconds
	trap
sys27loop:
	j	sys27loop

sys27fun:
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
