;
; sc51.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_ACCT,51

	.data

	; 51 = acct
sys51:
	add	$8,$0,SYS_ACCT
	add	$4,$0,path51
	trap
	add	$8,$0,SYS_ACCT
	add	$4,$0,0
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path51:
	.byte	"/WELCOME",0
