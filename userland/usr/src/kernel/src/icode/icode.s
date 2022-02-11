;
; icode -- initial code for process 1
;

	.set	SYS_EXECE,59		; exece system call

	.export	_start

	.code
	.align	4

	.nosyn

_start:
	add	$8,$0,SYS_EXECE		; number of system call
	add	$4,$0,name		; pointer to program name
	add	$5,$0,argv		; pointer to argument vector
	add	$6,$0,envp		; pointer to environment
	trap				; execute system call
error:					; should not return, but...
	j	error			; loop in case of error

argv:
	.word	name			; conventionally name of program
	.word	0			; terminated by null pointer

envp:
	.word	0			; terminated by null pointer

name:
	.byte	"/etc/init", 0		; name of program to execute
