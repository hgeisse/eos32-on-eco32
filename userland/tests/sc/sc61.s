;
; sc61.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_OPEN,5
	.set	SYS_CLOSE,6
	.set	SYS_CHROOT,61

	.data

	; 61 = chroot
sys61:
	add	$8,$0,SYS_CHROOT
	add	$4,$0,path61a
	trap
	add	$8,$0,SYS_OPEN
	add	$4,$0,path61b
	add	$5,$0,0			; mode = READ
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,$2		; use fd returned from open
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

path61a:
	.byte	"/boot",0

path61b:
	.byte	"/eos32.bin",0
