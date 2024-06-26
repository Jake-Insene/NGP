; --------------------------------------------
;         MEMORY IMMEDIATE INSTRUCTIONS
; --------------------------------------------
; RDX = Register List
; R10 = Dest/Source Index
; R11 = Base Index
; R14 = Disp
LD_IMMEDIATE_OPC:
	SAL R14D, 2
	MOV R11D, DWORD PTR [RDX+R11*4]
	ADD R11D, R14D

	MOV ECX, R11D
	CALL get_real_address

	RESTORE_ARGS

	MOV EAX, DWORD PTR[RAX]
	MOV DWORD PTR[RDX+R10*4], EAX

	JMP CONTINUE_ADDRESS
LDSH_IMMEDIATE_OPC:
	SAL R14D, 1
	MOV R11D, DWORD PTR [RDX+R11*4]
	ADD R11D, R14D

	MOV ECX, R11D
	CALL get_real_address

	RESTORE_ARGS

	MOV AX, WORD PTR[RAX]
	MOVSX EAX, AX
	MOV DWORD PTR[RDX+R10*4], EAX

	JMP CONTINUE_ADDRESS
LDH_IMMEDIATE_OPC:
	SAL R14D, 1
	MOV R11D, DWORD PTR [RDX+R11*4]
	ADD R11D, R14D

	MOV ECX, R11D
	CALL get_real_address

	RESTORE_ARGS

	MOV AX, WORD PTR[RAX]
	MOVZX EAX, AX
	MOV DWORD PTR[RDX+R10*4], EAX

	JMP CONTINUE_ADDRESS
LDSB_IMMEDIATE_OPC:
	MOV R11D, DWORD PTR [RDX+R11*4]
	ADD R11D, R14D

	MOV ECX, R11D
	CALL get_real_address

	RESTORE_ARGS

	MOV AL, BYTE PTR[RAX]
	MOVSX EAX, AL 
	MOV DWORD PTR[RDX+R10*4], EAX

	JMP CONTINUE_ADDRESS
LDB_IMMEDIATE_OPC:
	MOV R11D, DWORD PTR [RDX+R11*4]
	ADD R11D, R14D

	MOV ECX, R11D
	CALL get_real_address

	RESTORE_ARGS

	MOV AL, BYTE PTR[RAX]
	MOVZX EAX, AL
	MOV DWORD PTR[RDX+R10*4], EAX

	JMP CONTINUE_ADDRESS
ST_IMMEDIATE_OPC:
	SAL R14D, 2
	MOV R11D, DWORD PTR [RDX+R11*4]
	ADD R11D, R14D

	MOV ECX, R11D
	CALL get_real_address

	RESTORE_ARGS

	MOV R10D, DWORD PTR[RDX+R10*4]
	MOV DWORD PTR[RAX], R10D

	JMP CONTINUE_ADDRESS
STH_IMMEDIATE_OPC:
	SAL R14D, 1
	MOV R11D, DWORD PTR [RDX+R11*4]
	ADD R11D, R14D

	MOV ECX, R11D
	CALL get_real_address

	RESTORE_ARGS

	MOV R10W, WORD PTR[RDX+R10*4]
	MOV WORD PTR[RAX], R10W

	JMP CONTINUE_ADDRESS
STB_IMMEDIATE_OPC:
	MOV R11D, DWORD PTR [RDX+R11*4]
	ADD R11D, R14D

	MOV ECX, R11D
	CALL get_real_address

	RESTORE_ARGS

	MOV R10B, BYTE PTR[RDX+R10*4]
	MOV BYTE PTR[RAX], R10B

	JMP CONTINUE_ADDRESS

; --------------------------------------------
;     FLOAT MEMORY IMMEDIATE INSTRUCTIONS
; --------------------------------------------
; RDX = Register List
; R10 = Dest/Source Index
; R11 = Base Index
; R14 = Disp
LD_S_IMMEDIATE_OPC:
	JMP CONTINUE_ADDRESS
LD_D_IMMEDIATE_OPC:
	JMP CONTINUE_ADDRESS
LD_Q_IMMEDIATE_OPC:
	JMP CONTINUE_ADDRESS
ST_S_IMMEDIATE_OPC:
	JMP CONTINUE_ADDRESS
ST_D_IMMEDIATE_OPC:
	JMP CONTINUE_ADDRESS
ST_Q_IMMEDIATE_OPC:
	JMP CONTINUE_ADDRESS

; --------------------------------------------
;            MEMORY PAIR INSTRUCTIONS
; --------------------------------------------
; RDX = Register List
; R10 = Dest/Source Index
; R11 = Second Dest/Source Index
; R12 = Base Index
; R15 = Disp
LDP_OPC:
	JMP CONTINUE_ADDRESS
LDP_S_OPC:
	JMP CONTINUE_ADDRESS
LDP_D_OPC:
	JMP CONTINUE_ADDRESS
LDP_Q_OPC:
	JMP CONTINUE_ADDRESS
STP_OPC:
	JMP CONTINUE_ADDRESS
STP_S_OPC:
	JMP CONTINUE_ADDRESS
STP_D_OPC:
	JMP CONTINUE_ADDRESS
STP_Q_OPC:
	JMP CONTINUE_ADDRESS