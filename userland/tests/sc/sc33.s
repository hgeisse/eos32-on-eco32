;
; sc33.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_SETUID,23
	.set	SYS_ACCESS,33

	.data

	; 33 = access
sys33:
	add	$8,$0,SYS_SETUID
	add	$4,$0,0x1234
	trap
	add	$8,$0,SYS_ACCESS
	add	$4,$0,path33
	add	$5,$0,4			; mode = READ
	trap
	add	$8,$0,SYS_ACCESS
	add	$4,$0,path33
	add	$5,$0,1			; mode = EXECUTE
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path33:
	.byte	"/WELCOME",0
