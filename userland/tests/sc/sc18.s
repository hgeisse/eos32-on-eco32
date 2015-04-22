;
; sc18.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_STAT,18

	.data

	; 18 = stat
sys18:
	add	$8,$0,SYS_STAT
	add	$4,$0,path18
	add	$5,$0,statbuf18
	trap
	add	$2,$0,statbuf18		; to ease inspection of data
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path18:
	.byte	"/WELCOME",0

	.align	4
statbuf18:
	.space	11*4
