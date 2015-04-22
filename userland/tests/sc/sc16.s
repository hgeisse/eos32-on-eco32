;
; sc16.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_CHOWN,16
	.set	SYS_SYNC,36

	.data

	; 16 = chown
sys16:
	add	$8,$0,SYS_CHOWN
	add	$4,$0,path16
	add	$5,$0,12
	add	$6,$0,34
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path16:
	.byte	"/WELCOME",0
