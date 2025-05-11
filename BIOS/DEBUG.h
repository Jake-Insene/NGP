INCLUDE "IO.h"

; R0 addr
; R1 len
PRINT:
    MOV R2, DEBUG_FIFO & 0xFFFF
    MOVT R2, DEBUG_FIFO >> 16
    
.loop:
    LD R3, [R0]
    ST R3, [R2]
    ADD R0, R0, 1
    SUB R1, R1, 1
    CMP R1, 0
    BNE .loop

    RET
