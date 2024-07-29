FORMAT RAW AS 'bin'
ORG 0x00000000

B reset_entry
B interrupt_handler

; --------- Reset Table ---------
reset_entry:
	HLT


; --------- Interrupt Table ---------
interrupt_handler:
	MSR R0, intcode
	ERET
