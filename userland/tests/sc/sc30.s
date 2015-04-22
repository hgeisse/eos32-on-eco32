;
; sc30.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_TIME,13
	.set	SYS_UTIME,30
	.set	SYS_SYNC,36

	.data

	; 30 = utime
sys30:
	add	$8,$0,SYS_TIME
	trap
	sub	$2,$2,182*24*60*60
	stw	$2,$0,times30+4
	add	$2,$2,10*24*60*60
	stw	$2,$0,times30+0
	add	$8,$0,SYS_UTIME
	add	$4,$0,path30
	add	$5,$0,times30
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path30:
	.byte	"/WELCOME",0

	.align	4
times30:
	.word	0			; last access
	.word	0			; last update
