
; R0 Base Buffer Address
; R1 Size of buffer in bytes
MemoryZero:
.loop:
	STB ZR, [R0]
	ADD R0, R0, 1
	SUB R1, R1, 1
	CMP R1, 0
	BNE .loop

	RET
