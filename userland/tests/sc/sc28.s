;
; sc28.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_OPEN,5
	.set	SYS_CLOSE,6
	.set	SYS_FSTAT,28

	.data

	; 28 = fstat
sys28:
	add	$8,$0,SYS_OPEN
	add	$4,$0,path28
	add	$5,$0,1			; mode = WRITE
	trap
	add	$16,$0,$2		; save fd
	add	$8,$0,SYS_FSTAT
	add	$4,$0,$16		; fd
	add	$5,$0,statbuf28
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,$16		; fd
	trap
	add	$2,$0,statbuf28		; to ease inspection of data
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path28:
	.byte	"/WELCOME",0

	.align	4
statbuf28:
	.space	11*4
