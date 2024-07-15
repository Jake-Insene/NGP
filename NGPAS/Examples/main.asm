ENTRY main
ORG 0x20000000

INCLUDE "another.asm"

main:
	; Setting up the stack
	MOV SP, 0x0000
	MOVT SP, 0x2000
	BL another

	HLT
