.export clearfe
.export getfe
.export getrnd
.export setrnd

; void clearfe(unsigned int)
; clear exception bits requested in mask
clearfe:
  mvfs $8, 16              ; fetch current fpc
  and $4, $4, 0x000001F0   ; only exception bits are allowed
  xor $4, $4, 0xFFFFFFFF   ; invert mask 
  and $8, $8, $4           ; clear requested bits 
  mvts $8, 16              ; write to fpc
  jr $31

; unsigned int getfe(unsigned int)
getfe:
  mvfs $2, 16
  and $2, $2, $4           ; apply mask for requested bits
  and $2, $2, 0x000001F0   ; apply mask for exception bits
  jr $31

; void getrnd(void)
; get current rounding mode
getrnd:
  mvfs $2, 16
  and $2, $2, 0x00000003   ; apply mask for rounding mode
  jr $31

; void setrnd(unsigned int)
setrnd:
  mvfs $8, 16
  and $8, $8, ~0x00000003
  add $9, $0, $4
  and $9, $9, 0x00000003
  or $8, $8, $9
  mvts $8, 16
  jr $31
