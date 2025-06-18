INCLUDE "IO.h"
INCLUDE "MACROS.h"

; Enable debug output
EnableDebugConsole:
    RET

; Print the UTF-8 string at address R0, with R1 length
; R0 - String Address
; R1 - String Length
Print:
    RET

; Print the UTF-8 Character in R0
; R0 Character
PutChar:
    RET
