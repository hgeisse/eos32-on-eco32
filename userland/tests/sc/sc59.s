;
; sc59.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_FORK,2
	.set	SYS_WAIT,7
	.set	SYS_EXECE,59

	.data

	; 59 = exece
sys59:
	add	$8,$0,SYS_FORK
	trap
	bne	$2,$0,sys59a
	add	$8,$0,SYS_EXECE
	add	$4,$0,path59
	add	$5,$0,arg59
	add	$6,$0,env59
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,42
	trap
sys59a:
	add	$8,$0,SYS_WAIT
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap

arg59:
	.word	arg59a
	.word	arg59b
	.word	0

env59:
	.word	env59a
	.word	0

path59:
	.byte	"/tst/sc59x",0

arg59a:
	.byte	"sc59x",0

arg59b:
	.byte	"-a",0

env59a:
	.byte	"VAR1=value1",0
