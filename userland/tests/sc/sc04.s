;
; sc04.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_READ,3
	.set	SYS_WRITE,4
	.set	SYS_OPEN,5
	.set	SYS_CLOSE,6

	.data

	; 4 = write
sys04:
	add	$8,$0,SYS_OPEN
	add	$4,$0,path04
	add	$5,$0,1			; mode = WRITE
	trap
	add	$16,$0,$2		; save fd
	add	$8,$0,SYS_WRITE
	add	$4,$0,$16		; fd
	add	$5,$0,data04w		; buffer
	add	$6,$0,4*16		; number of bytes
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,$16		; fd
	trap
	add	$8,$0,SYS_OPEN
	add	$4,$0,path04
	add	$5,$0,0			; mode = READ
	trap
	add	$16,$0,$2		; save fd
	add	$8,$0,SYS_READ
	add	$4,$0,$16		; fd
	add	$5,$0,data04r		; buffer
	add	$6,$0,4*16		; number of bytes
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,$16		; fd
	trap
	add	$2,$0,data04r		; to ease inspection of data
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path04:
	.byte	"/WELCOME",0

data04w:
	.byte	"0123456789ABCDEF"
	.byte	"456789ABCDEF0123"
	.byte	"89ABCDEF01234567"
	.byte	"CDEF0123456789AB"

data04r:
	.byte	"                "
	.byte	"                "
	.byte	"                "
	.byte	"                "
