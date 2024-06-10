.entry main

main:
	; Setting up the stack
	mov sp, #0
	movt sp, #0x0800

	halt
