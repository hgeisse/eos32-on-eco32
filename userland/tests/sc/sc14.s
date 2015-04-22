;
; sc14.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_MKNOD,14
	.set	SYS_SYNC,36

	.data

	; 14 = mknod
sys14:
	add	$8,$0,SYS_MKNOD
	add	$4,$0,path14
	add	$5,$0,020666		; char special, mode = rw-rw-rw-
	add	$6,$0,(0x12<<16)|0x34
	trap
	add	$8,$0,SYS_SYNC
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path14:
	.byte	"/dev/myspecial",0
