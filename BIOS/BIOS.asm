FORMAT RAW AS 'BIN'
ORG 0x00000000
vector_address_table:
B main ; Reset Address/Entry point
B $ ; IRQ Handler
B $ ; Exception Handler
B $ ; Not Used

INCLUDE "DEBUG.h"
INCLUDE "DISPLAY.h"
INCLUDE "GU.h"
INCLUDE "MACROS.h"
INCLUDE "MEMORY.h"

COMMAND_LIST_SIZE = 0x1000

main:	
	; Setting Up SP
	ADR SP, SP_END
	SUB SP, SP, 0x10

	BL EnableDisplay

	MOV R0, 0 ; VRAM 0x00000000
	LD R1, CONFIG
	BL SetDisplayBuffer

.loop:
	ADR R0, CommandList
	MOV R1, COMMAND_LIST_SIZE/4
	MOV R2, GU_QUEUE_INDEX0
	MOV R3, GU_QUEUE_PRIORITY_NORMAL
	BL GUSendCommandList

	BL PresentDisplay
	B .loop
	HALT

CONFIG:
	.word DISPLAY_FORMAT_CREATE 0x100, 0x100, DISPLAY_FORMAT_RGBA8

VALUES:
	.float32 1.0, 2.0, 3.0, 4.0
VALUES2:
	.float64 1.0, 2.0, 3.0, 4.0

CommandList:
	.word 0x010000FF
	.word 0x00000000
	.word 0x00100010
	.zero COMMAND_LIST_SIZE

.align 256
my_image:
	INCBIN "my_image.bin"

.align 0x10
.zero 0x400000-$
SP_END: ; Stack pointer at the end of the BIOS