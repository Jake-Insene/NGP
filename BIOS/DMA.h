INCLUDE "IO.h"
INCLUDE "MACROS.h"

; Channels:
;	0: RAM Channel
;	1: EMD Channel

; R0 Dest VRAM Address
; R1 Src RAM Address
; R2 Word Count
DMASendGU:
	IMM32 R3, DMA_GU_CHANNEL

	ST R0, [R3, 4]
	ST R1, [R3, 8]
	ST R2, [R3, 12]

	IMM32 R1, 0x00000002
	ST R1, [R3]
	RET

; R0 Channel
DMASend:
	RET