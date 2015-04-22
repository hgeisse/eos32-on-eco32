;
; sc06.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_OPEN,5
	.set	SYS_CLOSE,6

	.data

	; 6 = close
sys06:
	add	$8,$0,SYS_OPEN
	add	$4,$0,path06
	add	$5,$0,0			; mode = READ
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,$2		; use fd returned from open
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path06:
	.byte	"/boot/eos32.bin",0
