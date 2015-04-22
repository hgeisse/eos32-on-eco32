;
; sc13.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_TIME,13

	.data

	; 13 = time
sys13:
	add	$8,$0,SYS_TIME
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
