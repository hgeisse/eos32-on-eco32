;
; sc15.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_CHMOD,15
	.set	SYS_SYNC,36

	.data

	; 15 = chmod
sys15:
	add	$8,$0,SYS_CHMOD
	add	$4,$0,path15
	add	$5,$0,0400
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path15:
	.byte	"/WELCOME",0
