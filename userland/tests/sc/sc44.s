;
; sc44.s -- system call test program
;

	.set	SYS_EXIT,1
	.set	SYS_PROFIL,44

	.data

	; 44 = profil
sys44:
	add	$8,$0,SYS_PROFIL
	add	$4,$0,0
	add	$5,$0,0
	add	$6,$0,0
	add	$7,$0,0
	trap
	add	$8,$0,SYS_EXIT
	add	$4,$0,0
	trap
