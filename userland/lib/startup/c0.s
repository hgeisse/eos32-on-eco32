;
; c0.s -- C startup (prolog)
;

	.import	main
	.import	exit

	.import	environ
	.import	curbrk

	.import	_ecode
	.import	_edata
	.import	_ebss

	.export	_bcode
	.export	_bdata
	.export	_bbss

	.code
_bcode:

	.data
_bdata:

	.bss
_bbss:

	.code
	.align	4

start:
	add	$4,$0,_ebss	; store current end of data space
	stw	$4,$0,curbrk
	ldw	$4,$29,0	; get argc
	add	$5,$29,4	; compute argv
	add	$6,$4,2		; compute envp
	sll	$6,$6,2
	add	$6,$29,$6
	stw	$6,$0,environ	; store envp
	sub	$29,$29,3*4	; space for main to store its args
	jal	main		; call main function
	add	$4,$2,$0	; returned value is argument for exit
	jal	exit		; this call never returns
