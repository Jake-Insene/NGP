INCLUDE "IO.h"

DISPLAY_ENABLE = 0x1

DISPLAY_FORMAT_RGB8 = 0x0
DISPLAY_FORMAT_RGBA8 = 0x1
DISPLAY_FORMAT_RGB565 = 0x2
DISPLAY_FORMAT_RGBA4 = 0x3


MACRO DISPLAY_FORMAT_CREATE Width, Height, Fmt{ (Width) | (Height << 14) | (Fmt << 28) }

EnableDisplay:
	IMM32 R0, DISPLAY_CTR
	MOV R1, DISPLAY_ENABLE
	ST R1, [R0]
	RET


; R0 Buffer Address
; R1 Display Format : W | H | FMT
CreateDisplay:
	IMM32 R2, DISPLAY_FORMAT
	ST R1, [R2]
	IMM32 R2, DISPLAY_BUFFER
	ST R0, [R2]
	RET


PresentDisplay:
	IMM32 R0, DISPLAY_PRESENT
	ST ZR, [R0]
	RET

; R0 0xAABBGGRR 32 bits color
; R1 Display Buffer Address
; R2 Display Buffer Size
ClearDisplay:
	MOV R3, 0

.loop:
	ST R0, [R1, R3]
	ADD R3, R3, 4
	CMP R3, R2
	BNE .loop

	RET
