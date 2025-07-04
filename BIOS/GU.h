INCLUDE "IO.h"
INCLUDE "MACROS.h"

GU_RESET = 0x1
GU_QUEUE_START = 0x80000000

GU_FORMAT_RGBA8 =		0x0
GU_FORMAT_RGB8 =		0x1
GU_FORMAT_RGBA4 =		0x2
GU_FORMAT_RGB565 =		0x3
GU_FORMAT_RGBA5551 =	0x4

; Command Layout
; 0xXXAABBCC
; XX: High 8 bits contains command type.
; AA, BB, CC: 8 bits arguments.

; 0x00XXXXXX
; Terminates the command list execution
; XXXXXX: Is ignored.
GU_CMD_END = 0x0


; Send a command list to the specific queue with the given priority and stack execution
; R0 Command Base Buffer Address
; R1 Buffer Length
GUSendCommandList:
	IMM32 R4, GU_QUEUE_ADDR
	ST R0, [R4]
	ST R1, [R4, 4]

	IMM32 R0, GU_QUEUE_START
	IMM32 R1, GU_QUEUE_CTR
	ST R0, [R1]

	RET
