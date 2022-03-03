.export resetfe
.export statusfe

; void resetfe(void)
resetfe:
  mvts $0, 16
  jr $31

; unsigned int statusfe(void)
; (bfranken TODO ggf Maske vorsehen: unsigned int statusfe(unsigned int) )
statusfe:
  mvfs $2, 16
  and $2, $2, 0x000001F0
  ; bfranken TODO ggf Maske 
  jr $31
