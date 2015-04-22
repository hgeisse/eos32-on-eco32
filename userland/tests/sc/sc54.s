;
; sc54.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_WRITE,4
	.set	SYS_OPEN,5
	.set	SYS_CLOSE,6
	.set	SYS_IOCTL,54

	.data

	; 54 = ioctl
sys54:
	add	$8,$0,SYS_OPEN		; open console
	add	$4,$0,path54
	add	$5,$0,2
	trap
	add	$16,$0,$2		; save fd
	add	$8,$0,SYS_WRITE		; write a message
	add	$4,$0,$16
	add	$5,$0,buff54
	add	$6,$0,45
	trap
	add	$8,$0,SYS_IOCTL		; get terminal control block
	add	$4,$0,$16
	add	$5,$0,('t'<<8)|8
	add	$6,$0,ttybuf54
	trap
	add	$8,$0,ttybuf54		; toggle case mapping flag
	ldw	$9,$8,4
	xor	$9,$9,0x0004
	stw	$9,$8,4
	add	$8,$0,SYS_IOCTL		; set terminal control block
	add	$4,$0,$16
	add	$5,$0,('t'<<8)|9
	add	$6,$0,ttybuf54
	trap
	add	$8,$0,SYS_WRITE		; write the message again
	add	$4,$0,$16
	add	$5,$0,buff54
	add	$6,$0,45
	trap
	add	$8,$0,SYS_CLOSE		; close console
	add	$4,$0,$16
	trap
	add	$8,$0,SYS_EXIT		; done
	add	$4,$0,0
	trap

path54:
	.byte	"/dev/console",0

buff54:
	.byte	"The quick brown fox jumps over the lazy dog.",0x0A

	.align	4
ttybuf54:
	.space	8
