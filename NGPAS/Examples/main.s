
; entry point of the program
; this will be my main function
.entry_point main

str:
	.string "Hello World"

main:
	mov r0, #0
	add r0, r0, #12
	sub r0, r0, #12
	halt
