; --------------------------------------------
;              LOAD STORE IMMEDIATE
; --------------------------------------------
; RDX = Register List
; R10 = Dest/Source Index
; R11 = Base Index
; R14 = Disp
LD_IMMEDIATE_OPC:
	SAL R14D, 2
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 0
	CALL memory_read

	JMP CONTINUE_ADDRESS
LDSH_IMMEDIATE_OPC:
	SAL R14D, 1
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 2
	CALL memory_read

	JMP CONTINUE_ADDRESS
LDH_IMMEDIATE_OPC:
	SAL R14D, 1
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 1
	CALL memory_read

	JMP CONTINUE_ADDRESS
LDSB_IMMEDIATE_OPC:
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 4
	CALL memory_read

	JMP CONTINUE_ADDRESS
LDB_IMMEDIATE_OPC:
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 3
	CALL memory_read

	JMP CONTINUE_ADDRESS
ST_IMMEDIATE_OPC:
	SAL R14D, 2
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 0
	CALL memory_write

	JMP CONTINUE_ADDRESS
STH_IMMEDIATE_OPC:
	SAL R14D, 1
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 1
	CALL memory_write

	JMP CONTINUE_ADDRESS
STB_IMMEDIATE_OPC:
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 3
	CALL memory_write

	JMP CONTINUE_ADDRESS

; --------------------------------------------
;           LOAD STORE FP IMMEDIATE
; --------------------------------------------
; RDX = Register List
; R10 = Dest/Source Index
; R11 = Base Index
; R14 = Disp
LD_S_IMMEDIATE_OPC:
	SAL R14D, 2
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 6
	CALL memory_read

	JMP CONTINUE_ADDRESS
LD_D_IMMEDIATE_OPC:
	SAL R14D, 3
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 7
	CALL memory_read

	JMP CONTINUE_ADDRESS
LD_Q_IMMEDIATE_OPC:
	SAL R14D, 4
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 5
	CALL memory_read

	JMP CONTINUE_ADDRESS
ST_S_IMMEDIATE_OPC:
	SAL R14D, 2
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 6
	CALL memory_write

	JMP CONTINUE_ADDRESS
ST_D_IMMEDIATE_OPC:
	SAL R14D, 3
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 7
	CALL memory_write

	JMP CONTINUE_ADDRESS
ST_Q_IMMEDIATE_OPC:
	SAL R14D, 4
	GET_REGISTER R11D, R11
	ADD R11D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8, 5
	CALL memory_write

	JMP CONTINUE_ADDRESS


; --------------------------------------------
;              LOAD STORE REGISTER
; --------------------------------------------
; RDX = Register List
; R10 = Dest/Source Index
; R11 = Base Index
; R12 = Inde
LD_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 0
    CALL memory_read

    JMP CONTINUE_ADDRESS
LDSH_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 2
    CALL memory_read

    JMP CONTINUE_ADDRESS
LDH_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 1
    CALL memory_read

    JMP CONTINUE_ADDRESS
LDSB_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 4
    CALL memory_read

    JMP CONTINUE_ADDRESS
LDB_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 3
    CALL memory_read

    JMP CONTINUE_ADDRESS
ST_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 0
    CALL memory_write

    JMP CONTINUE_ADDRESS
STH_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 1
    CALL memory_write


    JMP CONTINUE_ADDRESS
STB_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 3
    CALL memory_write

    JMP CONTINUE_ADDRESS
LD_S_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 6
    CALL memory_read

	JMP CONTINUE_ADDRESS
LD_D_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 7
    CALL memory_read

	JMP CONTINUE_ADDRESS
LD_Q_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 5
    CALL memory_read

    JMP CONTINUE_ADDRESS
ST_S_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 6
    CALL memory_write

	JMP CONTINUE_ADDRESS
ST_D_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 7
    CALL memory_write

	JMP CONTINUE_ADDRESS
ST_Q_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 5
    CALL memory_write

	JMP CONTINUE_ADDRESS

; --------------------------------------------
;                 LOAD STORE PAIR
; --------------------------------------------
; RDX = Register List
; R10 = Dest/Source Index
; R11 = Second Dest/Source Index
; R12 = Base Index
; R14 = Disp
LDP_OPC:
	SAL R14D, 3
	GET_REGISTER R12D, R12
	ADD R12D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8D, R12D
	MOV R9D, 0
	CALL memory_read_pair

	JMP CONTINUE_ADDRESS
LDP_S_OPC:
	SAL R14D, 3
	GET_REGISTER R12D, R12
	ADD R12D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8D, R12D
	MOV R9D, 6
	CALL memory_read_pair

	JMP CONTINUE_ADDRESS
LDP_D_OPC:
	SAL R14D, 4
	GET_REGISTER R12D, R12
	ADD R12D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8D, R12D
	MOV R9D, 6
	CALL memory_read_pair

	JMP CONTINUE_ADDRESS
LDP_Q_OPC:
	SAL R14D, 5
	GET_REGISTER R12D, R12
	ADD R12D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8D, R12D
	MOV R9D, 5
	CALL memory_read_pair

	JMP CONTINUE_ADDRESS
STP_OPC:
	SAL R14D, 3
	GET_REGISTER R12D, R12
	ADD R12D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8D, R12D
	MOV R9D, 0
	CALL memory_write_pair

	JMP CONTINUE_ADDRESS
STP_S_OPC:
	SAL R14D, 3
	GET_REGISTER R12D, R12
	ADD R12D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8D, R12D
	MOV R9D, 6
	CALL memory_write_pair

	JMP CONTINUE_ADDRESS
STP_D_OPC:
	SAL R14D, 4
	GET_REGISTER R12D, R12
	ADD R12D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8D, R12D
	MOV R9D, 7
	CALL memory_write_pair

	JMP CONTINUE_ADDRESS
STP_Q_OPC:
	SAL R14D, 5
	GET_REGISTER R12D, R12
	ADD R12D, R14D

	MOV ECX, R10D
	MOV EDX, R11D
	MOV R8D, R12D
	MOV R9D, 5
	CALL memory_write_pair

	JMP CONTINUE_ADDRESS