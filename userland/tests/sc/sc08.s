;
; sc08.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_WRITE,4
	.set	SYS_CLOSE,6
	.set	SYS_CREAT,8
	.set	SYS_SYNC,36

	.data

	; 8 = creat
sys08:
	add	$8,$0,SYS_CREAT
	add	$4,$0,path08
	add	$5,$0,0644		; mode = rw-r--r--
	trap
	add	$16,$0,$2		; save fd
	add	$8,$0,SYS_WRITE
	add	$4,$0,$16		; fd
	add	$5,$0,buffer08		; buffer
	add	$6,$0,27		; number of bytes
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,$16		; fd
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path08:
	.byte	"/boot/README",0

buffer08:
	.byte	"This is the boot directory."
