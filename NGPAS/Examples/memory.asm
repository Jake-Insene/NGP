
; entry point of the program
; this will be my main function
entry main

my_var: ; global variable
	.word #0
	.word #0
main:
	; setting up the sp pointer
	mov sp, #0x0000
	movt sp, #0x1000
	
	; stack allocation
	sub sp, sp, #16
	
	mov r0, #0xff
	st r0, [sp]
	st r0, [sp, #4]

	; getting the address of the global
	adr r1, my_var
	ld r0, [r1]
	ld r2, [r1, #4]

	st r0, [sp]
	st r2, [sp, #4]	

	; stack deallocation
	add sp, sp, #16

	halt
