;
; sc09.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_LINK,9
	.set	SYS_SYNC,36

	.data

	; 9 = link
sys09:
	add	$8,$0,SYS_LINK
	add	$4,$0,path09old
	add	$5,$0,path09new
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path09old:
	.byte	"/WELCOME",0

path09new:
	.byte	"/README",0
