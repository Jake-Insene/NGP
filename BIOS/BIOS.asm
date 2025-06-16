FORMAT RAW AS 'BIN'
ORG 0x00000000
vector_address_table:
B main ; Reset Address/Entry point
B $ ; IRQ Handler
B $ ; Exception Handler
B $ ; Not Used

INCLUDE "DEBUG.h"
INCLUDE "DISPLAY.h"
INCLUDE "DMA.h"
INCLUDE "GU.h"
INCLUDE "MACROS.h"
INCLUDE "MEMORY.h"

COMMAND_LIST_SIZE = 0x1000
DISPLAY_CONFIG = DISPLAY_FORMAT_CREATE 0x100, 0x100, DISPLAY_FORMAT_RGBA8
DISPLAY_BUFFER_ADDR = 0
IMAGE_UPLOAD_ADDR = DISPLAY_BUFFER_ADDR + (0x100 * 0x100 * 4)

main:	
	; Setting Up SP
	ADR SP, SP_END
	SUB SP, SP, 0x10

	BL EnableDisplay

	MOV R0, 0 ; VRAM 0x00000000
	IMM32 R1, DISPLAY_CONFIG
	BL SetDisplayBuffer

	IMM32 R0, IMAGE_UPLOAD_ADDR
	ADR R1, my_image
	MOV R2, 64
	BL DMASendGU

.loop:
	ADR R0, CommandListBegin
	MOV R1, (CommandListEnd - CommandListBegin) >> 2
	MOV R2, GU_QUEUE_INDEX0
	MOV R3, GU_QUEUE_PRIORITY_NORMAL
	BL GUSendCommandList

	BL PresentDisplay
	B .loop
	HALT

CommandListBegin:
	.word 0x01000000
	.word 0x00000000
	.word 0x01000100
	.word 0x02000000 | IMAGE_UPLOAD_ADDR >> 8
	.word 0x00000000
	.word 0x00060006
CommandListEnd:

.align 4
my_image:
	INCBIN "my_image.bin"

.align 0x10
.zero 0x400000-$
SP_END: ; Stack pointer at the end of the BIOS