;
; sc21.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_WRITE,4
	.set	SYS_CLOSE,6
	.set	SYS_CREAT,8
	.set	SYS_MOUNT,21
	.set	SYS_UMOUNT,22

	.data

	; 21 = mount
sys21:
	add	$8,$0,SYS_MOUNT
	add	$4,$0,special21
	add	$5,$0,mntpnt21
	add	$6,$0,0			; allow writing
	trap
	add	$8,$0,SYS_CREAT
	add	$4,$0,path21
	add	$5,$0,0644		; mode = rw-r--r--
	trap
	add	$16,$0,$2		; save fd
	add	$8,$0,SYS_WRITE
	add	$4,$0,$16		; fd
	add	$5,$0,buffer21		; buffer
	add	$6,$0,33		; number of bytes
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,$16		; fd
	trap
	add	$8,$0,SYS_UMOUNT
	add	$4,$0,special21
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

special21:
	.byte	"/dev/disk-2",0

mntpnt21:
	.byte	"/usr",0

path21:
	.byte	"/usr/hellwig/README",0

buffer21:
	.byte	"This is Hellwig's home directory."
