FORMAT RAW AS 'BIN'
ORG 0x00000000
vector_address_table:
B main
.word 0
.word 0
.word 0

INCLUDE "IO.h"
INCLUDE "DEBUG.h"

main:
	; Setting Up SP
	ADR SP, SP_END

	; Config Display Output
	MOV R0, GU_DISPLAYADDR & 0xFFFF
	MOVT R0, GU_DISPLAYADDR >> 16
	ST ZR, [R0]
	LD R1, CONFIG
	MOV R0, GU_DISPLAYFMT & 0xFFFF
	MOVT R0, GU_DISPLAYFMT >> 16
	ST R1, [R0]

	MOV R0, DEBUG_CTR & 0xFFFF
	MOVT R0, DEBUG_CTR >> 16
	MOV R1, 0x1
	ST R1, [R0]

	ADR R0, STRING
	MOV R1, 12
	BL PRINT

	HALT

CONFIG:
.word 0x100 | (0x100 << 12) | (0 << 16)
STRING:
.string "HELLO WORLD\n"

SP_END:
.zero 0x100 ; 256 bytes -> 64 words
.zero 0x400000-$