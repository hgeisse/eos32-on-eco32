.export _resetfe
.export statusfe
.export setrnd

; void _resetfe(unsigned int)
; clear exception bits requested in mask
_resetfe:
  mvfs $8, 16              ; fetch current fpc
  and $4, $4, 0x000001F0   ; only exception bits are allowed
  xor $4, $4, 0xFFFFFFFF   ; invert mask 
  and $8, $8, $4           ; clear requested bits 
  mvts $8, 16              ; write to fpc
  jr $31

; unsigned int statusfe(void)
; (bfranken TODO ggf Maske vorsehen: unsigned int statusfe(unsigned int) )
statusfe:
  mvfs $2, 16
  and $2, $2, 0x000001F0
  ; bfranken TODO ggf Maske 
  jr $31

; void setrnd(unsigned int)
setrnd:
  ; TODO bfranken: for rounding modes values > 3 are not allowed
  ; TODO should I check for that? currently I silently ignore 
  ;      everything but the last two bits
  mvfs $8, 16
  and $8, $8, ~0x00000003
  add $9, $0, $4
  and $9, $9, 0x00000003
  or $8, $8, $9
  mvts $8, 16
  jr $31
