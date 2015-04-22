;
; sc12.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_OPEN,5
	.set	SYS_CLOSE,6
	.set	SYS_CHDIR,12

	.data

	; 12 = chdir
sys12:
	add	$8,$0,SYS_CHDIR
	add	$4,$0,path12a
	trap
	add	$8,$0,SYS_OPEN
	add	$4,$0,path12b
	add	$5,$0,0			; mode = READ
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,$2		; use fd returned from open
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path12a:
	.byte	"/boot",0

path12b:
	.byte	"eos32.bin",0
