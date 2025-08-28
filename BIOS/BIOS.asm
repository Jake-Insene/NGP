FORMAT RAW AS 'BIN'
ORG 0x00000000
vector_address_table_el1:
	B main ; Reset Address/Entry point
	B exception_handler ; Exception Handler
	B interrupt_handler ; IRQ Handler
	B $ ; Not Used

exception_handler:
	ERET
interrupt_handler:
	ERET

INCLUDE "DEBUG.h"
INCLUDE "DISPLAY.h"
INCLUDE "DMA.h"
INCLUDE "GU.h"
INCLUDE "MACROS.h"
INCLUDE "MEMORY.h"

DISPLAY_WIDTH = 0x100
DISPLAY_HEIGHT = 0x100
DISPLAY_CONFIG = DISPLAY_FORMAT_CREATE DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_FORMAT_RGBA8

main:
	; Setting Up SP
	ADR SP, SP_END
	SUB SP, SP, 0x10

	ADR R0, vector_address_table_el1
	MSR vbar_el1, R0

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
	SHR R4, R0, 8
	SHL R2, R2, 24
	OR R2, R2, R4
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
	BL GUSendCommandList

	BL PresentDisplay
	B .loop
	HALT

CommandListBegin:
	.word 0
	.word 0
	.word 0
	.word 0x210000FF
	.word 0x00000000
	.word 0x01000100
	.word 0x2200FF00
	.word 0x00000000
	.word 0x00000010
	.word 0x00100010
	.zero 0x1000
CommandListEnd:

.align 4
DoubleBuffer:
	.word 0x00000000 ; First frame buffer
	.word 0x00040000 ; Second frame buffer

.align 0x10
.zero 0x400000-$
SP_END: ; Stack pointer at the end of the BIOS