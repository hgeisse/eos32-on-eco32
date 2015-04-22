;
; sc42.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_FORK,2
	.set	SYS_READ,3
	.set	SYS_WRITE,4
	.set	SYS_CLOSE,6
	.set	SYS_PIPE,42

	.data

	; 42 = pipe
sys42:
	add	$8,$0,SYS_PIPE
	trap
	add	$16,$0,$2		; read file descriptor
	add	$17,$0,$3		; write file descriptor
	add	$8,$0,SYS_FORK
	trap
	bne	$2,$0,sys42a
	add	$8,$0,SYS_CLOSE		; child closes read fd
	add	$4,$0,$16
	trap
	add	$8,$0,SYS_WRITE		; child writes message
	add	$4,$0,$17
	add	$5,$0,buf42w
	add	$6,$0,13
	trap
	add	$8,$0,SYS_EXIT		; child is done
	add	$4,$0,0
	trap
sys42a:
	add	$8,$0,SYS_CLOSE		; parent closes write fd
	add	$4,$0,$17
	trap
	add	$8,$0,SYS_READ		; parent reads message
	add	$4,$0,$16
	add	$5,$0,buf42r
	add	$6,$0,16
	trap
	add	$2,$0,buf42r		; to ease inspection
	add	$8,$0,SYS_EXIT		; parent is done
	add	$4,$0,0
	trap

buf42w:
	.byte	"Hello, world!"

buf42r:
	.byte	"0123456789ABCDEF"
