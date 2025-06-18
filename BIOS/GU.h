INCLUDE "IO.h"
INCLUDE "MACROS.h"

GU_RESTART = 0x1
GU_QUEUE_START = 0x80000000

GU_QUEUE_PRIORITY_LOW = 0x0
GU_QUEUE_PRIORITY_NORMAL = 0x1
GU_QUEUE_PRIORITY_HIGH = 0x2

GU_QUEUE_INDEX0 = 0x0
GU_QUEUE_INDEX1 = 0x1
GU_QUEUE_INDEX2 = 0x2
GU_QUEUE_INDEX3 = 0x3

GU_QUEUE_EXEC_STATE_FREE = 0x0
GU_QUEUE_EXEC_STATE_BUSY = 0x1

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
; R2 Queue Index
; R3 Queue Priority
GUSendCommandList:
	IMM32 R4, GU_QUEUE_ADDR
	ST R0, [R4]
	ST R1, [R4, 4]

	IMM32 R0, GU_QUEUE_START
	OR R0, R0, R3
	SHL R2, R2, 2
	OR R0, R0, R2
	IMM32 R1, GU_QUEUE_CTR
	ST R0, [R1]

	RET
