INCLUDE "IO.h"
INCLUDE "MACROS.h"

; Channels:
;	0: RAM Channel
;	1: USI Channel
;	2: SPU Channel
;	3: GU Channel

DMA_RAM = 0x0
DMA_USI = 0x1
DMA_SPU = 0x2
DMA_GU = 0x3

DMA_START = 0x1
DMA_BUSY = 0x1
DMA_FILL = 0x40000000
DMA_IRQ = 0x80000000

; Send data in words to the specific DMA channel
; R0 DMA Channel
; R1 Dest Device Address
; R2 Src RAM Address
; R3 Word Count
DMASend:
	IMM32 R4, DMA_BASE
	SHL R0, R0, 4
	ADD R4, R4, R0

	ST R1, [R4, 4]
	ST R2, [R4, 8]
	ST R3, [R4, 12]
	
	IMM32 R0, DMA_START
	ST R0, [R4]
	RET

