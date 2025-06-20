FORMAT RAW AS 'BIN'
ORG 0x00000000
vector_address_table_el0:
	B main ; Reset Address/Entry point
	B $ ; Exception Handler
	B $ ; IRQ Handler
	B $ ; Not Used

INCLUDE "DEBUG.h"
INCLUDE "DISPLAY.h"
INCLUDE "DMA.h"
INCLUDE "GU.h"
INCLUDE "MACROS.h"
INCLUDE "MEMORY.h"

DISPLAY_CONFIG = DISPLAY_FORMAT_CREATE 0x100, 0x100, DISPLAY_FORMAT_RGBA8
DISPLAY_BUFFER_ADDR = 0
IMAGE_UPLOAD_ADDR = DISPLAY_BUFFER_ADDR + (0x100 * 0x100 * 4)

main:	
	; Setting Up SP
	ADR SP, SP_END
	SUB SP, SP, 0x10

	BL EnableDisplay

	MOV R0, 0
	ST R0, [SP]
.loop:
	LD R0, [SP]
	EOR R1, R0, 1
	ST R1, [SP]

	ADR R2, DoubleBuffer
	SHL R0, R0, 2
	SHL R1, R1, 2
	; Draw Buffer
	LD R0, [R2, R0]
	; Display Buffer
	LD R1, [R2, R1]
	
	ADR R3, CommandListBegin
	MOV R2, 0x01
	SHL R2, R2, 24
	OR R2, R2, R0, SHR 8
	ST R2, [R3]

	IMM32 R2, 0x00100100
	ST R2, [R3, 4]
	IMM32 R2, 0x00000000
	ST R2, [R3, 8]

	MOV R0, R1
	IMM32 R1, DISPLAY_CONFIG
	BL SetDisplayBuffer

	ADR R0, CommandListBegin
	MOV R1, (CommandListEnd - CommandListBegin) >> 2
	MOV R2, GU_QUEUE_INDEX0
	MOV R3, GU_QUEUE_PRIORITY_NORMAL
	BL GUSendCommandList

	BL PresentDisplay
	B .loop
	HALT

CommandListBegin:
	.word 0
	.word 0
	.word 0
	.word 0x200000FF
	.word 0x00000000
	.word 0x01000100
	.word 0x2100FF00
	.word 0x00000000
	.word 0x00000001
	.word 0x00010001
	.zero 0x1000
CommandListEnd:

DoubleBuffer:
	.word 0x00000000 ; First frame buffer
	.word 0x00040000 ; Second framebuffer

.align 0x10
.zero 0x400000-$
SP_END: ; Stack pointer at the end of the BIOS