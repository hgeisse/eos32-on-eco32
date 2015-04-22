;
; sc19.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_READ,3
	.set	SYS_OPEN,5
	.set	SYS_CLOSE,6
	.set	SYS_SEEK,19
	.set	SYS_MOUNT,21
	.set	SYS_UMOUNT,22

	.data

	; 19 = seek
sys19:
	add	$8,$0,SYS_MOUNT
	add	$4,$0,special19
	add	$5,$0,mntpnt19
	add	$6,$0,1			; read-only
	trap
	add	$8,$0,SYS_OPEN
	add	$4,$0,path19
	add	$5,$0,0			; read
	trap
	add	$16,$0,$2		; save fd
	add	$8,$0,SYS_SEEK
	add	$4,$0,$16		; fd
	add	$5,$0,0x00003124	; offset (loc in direct blk)
	add	$6,$0,0			; whence (from start)
	trap
	add	$8,$0,SYS_READ
	add	$4,$0,$16		; fd
	add	$5,$0,buffer19a		; buffer
	add	$6,$0,4			; number of bytes
	trap
	add	$8,$0,SYS_SEEK
	add	$4,$0,$16		; fd
	add	$5,$0,0x00321124	; offset (loc in single indir blk)
	add	$6,$0,0			; whence (from start)
	trap
	add	$8,$0,SYS_READ
	add	$4,$0,$16		; fd
	add	$5,$0,buffer19b		; buffer
	add	$6,$0,4			; number of bytes
	trap
	add	$8,$0,SYS_SEEK
	add	$4,$0,$16		; fd
	add	$5,$0,0x00654124	; offset (loc in double indir blk)
	add	$6,$0,0			; whence (from start)
	trap
	add	$8,$0,SYS_READ
	add	$4,$0,$16		; fd
	add	$5,$0,buffer19c		; buffer
	add	$6,$0,4			; number of bytes
	trap
	add	$8,$0,SYS_CLOSE
	add	$4,$0,$16		; fd
	trap
	add	$8,$0,SYS_UMOUNT
	add	$4,$0,special19
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

special19:
	.byte	"/dev/disk-2",0

mntpnt19:
	.byte	"/usr",0

path19:
	.byte	"/usr/data/800",0

	.align	4
buffer19a:
	.word	0			; should be filled with 0x00000003
buffer19b:
	.word	0			; should be filled with 0x00000321
buffer19c:
	.word	0			; should be filled with 0x00000654
