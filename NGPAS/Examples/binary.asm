
; entry point of the program
; this will be my main function
entry main

main:
	mov r0, #255
	add r0, r0, #1
	sub r0, r0, #300
	
	mov r1, #10
	mul r0, r0, r1
	umul r0, r0, r1
	
	mov r1, #2
	div r0, r0, r1
	udiv r0, r0, r1

	and r0, r1, #0xFF
	or r0, r1, #0x10
	xor r0, r1, r0
	shl r0, r1, #2
	shr r0, r1, #6

	not r0, r1
	neg r0, r1
	abs r0, r1

	cmp r0, r1
	cmp r0, #0xFF
	
	mov r0, #1
	mov r1, #2

loop:
	cmp r0, r1
	bne end
	sub r1, r1, #1
	b loop
end:
	
	halt
