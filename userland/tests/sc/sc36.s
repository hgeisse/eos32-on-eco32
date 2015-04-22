;
; sc36.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_WRITE,4
	.set	SYS_OPEN,5
	.set	SYS_CLOSE,6
	.set	SYS_SYNC,36

	.data

	; 36 = sync
sys36:
	add	$8,$0,SYS_OPEN
	add	$4,$0,path36
	add	$5,$0,1			; mode = WRITE
	trap
	add	$16,$0,$2		; save fd
	add	$8,$0,SYS_WRITE
	add	$4,$0,$16		; fd
	add	$5,$0,data36w		; buffer
	add	$6,$0,4*16		; number of bytes
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,$16		; fd
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path36:
	.byte	"/WELCOME",0

data36w:
	.byte	"0123456789ABCDEF"
	.byte	"456789ABCDEF0123"
	.byte	"89ABCDEF01234567"
	.byte	"CDEF0123456789AB"
