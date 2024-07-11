ENTRY main

INCLUDE "another.asm"

main:
	; Setting up the stack
	MOV SP, #0x0000
	MOVT SP, #0x1000
	BL another

	HLT
