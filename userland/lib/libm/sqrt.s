.export sqrt
.align 4
.code

; float sqrt(float);
sqrt:
  sqrt $2, $4
  j $31

