FORMAT RAW AS 'BIN'
ORG 0x00000000
vector_address_table:
B main ; Reset Address/Entry point
.word 0 ; IRQ Handler
.word 0 ; Exception Handler
.word 0 ; Not Used

INCLUDE "DEBUG.h"
INCLUDE "DISPLAY.h"
INCLUDE "GU.h"
INCLUDE "MACROS.h"

main:	
	; Setting Up SP
	ADR SP, SP_END

	BL EnableDisplay

	ADR R0, DisplayBuffer
	LD R1, CONFIG
	BL CreateDisplay

	BL DisplayPresent

	HALT

CONFIG:
.word DISPLAY_FORMAT_CREATE 0x100, 0x100, DISPLAY_FORMAT_RGBA8

DisplayBuffer:
	.word 0xFFFFFFFF
	.zero 0x40000

.align 0x10
.zero 0x400000-$
SP_END: ; Stack pointer at the end of the BIOS