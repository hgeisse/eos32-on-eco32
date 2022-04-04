.export fabs
.align 4
.code

; float fabs(float);
fabs:
  absf $2, $4
  j $31

