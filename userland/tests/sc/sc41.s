;
; sc41.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_READ,3
	.set	SYS_OPEN,5
	.set	SYS_CLOSE,6
	.set	SYS_DUP,41

	.data

	; 41 = dup
sys41:
	add	$8,$0,SYS_OPEN
	add	$4,$0,path41
	add	$5,$0,0			; mode = READ
	trap
	add	$16,$0,$2		; save original fd
	add	$8,$0,SYS_DUP
	add	$4,$0,$16		; original fd
	add	$5,$0,10		; new fd = 10
	add	$6,$0,1			; this is a "dup2"
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,$16		; original fd
	trap
	add	$8,$0,SYS_READ
	add	$4,$0,10		; new fd = 10
	add	$5,$0,data41		; buffer
	add	$6,$0,4*16		; number of bytes
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,10		; new fd = 10
	trap
	add	$2,$0,data41		; to ease inspection of data
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path41:
	.byte	"/WELCOME",0

data41:
	.byte	"0123456789ABCDEF"
	.byte	"0123456789ABCDEF"
	.byte	"0123456789ABCDEF"
	.byte	"0123456789ABCDEF"
