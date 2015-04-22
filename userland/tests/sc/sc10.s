;
; sc10.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_LINK,9
	.set	SYS_UNLINK,10
	.set	SYS_SYNC,36

	.data

	; 10 = unlink
sys10:
	add	$8,$0,SYS_LINK
	add	$4,$0,path10old
	add	$5,$0,path10new
	trap
	add	$8,$0,SYS_UNLINK
	add	$4,$0,path10old
	trap
	add	$8,$0,SYS_UNLINK
	add	$4,$0,path10new
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path10old:
	.byte	"/WELCOME",0

path10new:
	.byte	"/README",0
