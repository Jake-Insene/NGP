.DATA

MAIN_OPCODE:
	dq BL_OPC
    dq BRANCH_OPC
    dq SWI_OPC
    dq BRANCH_COND_OPC
    dq LOGICAL_ADD_SUB_OPC
    dq FBINARY_OPC
    dq MEMORY_IMMEDIATE_OPC
    dq FMEMORY_IMMEDIATE_OPC
    dq MEMORY_PAIR_OPC
    dq ADDITIONAL_OPC
    dq NON_BINARY_OPC
    dq LD_PC_OPC
    dq LD_S_PC_OPC
    dq LD_D_PC_OPC
    dq LD_Q_PC_OPC
    dq ADR_PC_OPC
    dq IMMEDIATE_OPC
    dq ADD_IMMEDIATE_OPC
    dq ADDS_IMMEDIATE_OPC
    dq SUB_IMMEDIATE_OPC
    dq SUBS_IMMEDIATE_OPC
    dq AND_IMMEDIATE_OPC
    dq ANDS_IMMEDIATE_OPC
    dq OR_IMMEDIATE_OPC
    dq ORN_IMMEDIATE_OPC
    dq EOR_IMMEDIATE_OPC
    dq TBZ_OPC
    dq TBNZ_OPC
    dq CBZ_OPC
    dq CBNZ_OPC

BRANCH_CONDITIONAL_OPCODE:
    dq BEQ_OPC
    dq BNE_OPC
    dq BLT_OPC
    dq BLE_OPC
    dq BGT_OPC
    dq BGE_OPC
    dq BC_OPC
    dq BNC_OPC
    dq BN_OPC
    dq BP_OPC
    dq BO_OPC
    dq BNO_OPC
    dq BHI_OPC
    dq BLS_OPC

LOGICAL_ADD_SUB_OPCODE:
    dq ADD_SHL_OPC
    dq ADD_SHR_OPC
    dq ADD_ASR_OPC
    dq ADC_OPC
    dq SUB_SHL_OPC
    dq SUB_SHR_OPC
    dq SUB_ASR_OPC
    dq SBC_OPC
    dq AND_SHL_OPC
    dq AND_SHR_OPC
    dq AND_ASR_OPC
    dq AND_ROR_OPC
    dq OR_SHL_OPC
    dq OR_SHR_OPC
    dq OR_ASR_OPC
    dq OR_ROR_OPC
    dq ORN_SHL_OPC
    dq ORN_SHR_OPC
    dq ORN_ASR_OPC
    dq ORN_ROR_OPC
    dq EOR_SHL_OPC
    dq EOR_SHR_OPC
    dq EOR_ASR_OPC
    dq EOR_ROR_OPC
    dq ADDS_SHL_OPC
    dq ADDS_SHR_OPC
    dq ADDS_ASR_OPC
    dq ADDS_ROR_OPC
    dq SUBS_SHL_OPC
    dq SUBS_SHR_OPC
    dq SUBS_ASR_OPC
    dq SUBS_ROR_OPC
    dq ANDS_SHL_OPC
    dq ANDS_SHR_OPC
    dq ANDS_ASR_OPC
    dq ANDS_ROR_OPC
    dq BIC_SHL_OPC
    dq BIC_SHR_OPC
    dq BIC_ASR_OPC
    dq BIC_ROR_OPC
    dq BICS_SHL_OPC
    dq BICS_SHR_OPC
    dq BICS_ASR_OPC
    dq BICS_ROR_OPC
    dq ADCS_OPC
    dq SBCS_OPC

FBINARY_OPCODE:
    dq FMOV_S_OPC
    dq FMOV_D_OPC
    dq FMOV_NC_W_S_OPC
    dq FMOV_NC_S_W_OPC
    dq FMOV_NC_W_D_OPC
    dq FMOV_NC_D_W_OPC
    dq FCVTZS_S_OPC
    dq FCVTZU_S_OPC
    dq FCVTZS_D_OPC
    dq FCVTZU_D_OPC
    dq SCVTF_S_OPC
    dq UCVTF_S_OPC
    dq SCVTF_D_OPC
    dq UCVTF_D_OPC
    dq FADD_S_OPC
    dq FSUB_S_OPC
    dq FMUL_S_OPC
    dq FDIV_S_OPC
    dq FADD_D_OPC
    dq FSUB_D_OPC
    dq FMUL_D_OPC
    dq FDIV_D_OPC
    dq LD_S_OPC
    dq LD_D_OPC
    dq LD_Q_OPC
    dq ST_S_OPC
    dq ST_D_OPC
    dq ST_Q_OPC
    dq FNEG_S_OPC
    dq FNEG_D_OPC
    dq FABS_S_OPC
    dq FABS_D_OPC

MEMORY_IMMEDIATE_OPCODE:
    dq LD_IMMEDIATE_OPC
    dq LDSH_IMMEDIATE_OPC
    dq LDH_IMMEDIATE_OPC
    dq LDSB_IMMEDIATE_OPC
    dq LDB_IMMEDIATE_OPC
    dq ST_IMMEDIATE_OPC
    dq STH_IMMEDIATE_OPC
    dq STB_IMMEDIATE_OPC

FMEMORY_IMMEDIATE_OPCODE:
    dq LD_S_IMMEDIATE_OPC
    dq LD_D_IMMEDIATE_OPC
    dq LD_Q_IMMEDIATE_OPC
    dq ST_S_IMMEDIATE_OPC
    dq ST_D_IMMEDIATE_OPC
    dq ST_Q_IMMEDIATE_OPC

MEMORY_PAIR_OPCODE:
    dq LDP_OPC
    dq LDP_S_OPC
    dq LDP_D_OPC
    dq LDP_Q_OPC
    dq STP_OPC
    dq STP_S_OPC
    dq STP_D_OPC
    dq STP_Q_OPC

ADDITIONAL_OPCODE:
    dq LD_OPC
    dq LDSH_OPC
    dq LDH_OPC
    dq LDSB_OPC
    dq LDB_OPC
    dq ST_OPC
    dq STH_OPC
    dq STB_OPC
    dq MADD_OPC
    dq MSUB_OPC
    dq UMADD_OPC
    dq UMSUB_OPC
    dq DIV_OPC
    dq UDIV_OPC
    dq SHL_OPC
    dq SHR_OPC
    dq ASR_OPC
    dq ROR_OPC
    dq ABS_OPC

NON_BINARY_OPCODE:
    dq RET_OPC
    dq HALT_OPC

IMMEDIATE_OPCODE:
    dq MOVT_IMMEDIATE_OPC
    dq MVN_IMMEDIATE_OPC
    dq FMOV_S_IMMEDIATE_OPC
    dq FMOV_D_IMMEDIATE_OPC

; --------------------------------------------
;               MAIN INSTRUCTIONS
; --------------------------------------------

.CODE
INCLUDE MacrosWIN64.inc
INCLUDE LogicalAddSubWIN64.inc
INCLUDE BranchConditionalWIN64.inc
INCLUDE FBinaryWIN64.inc
INCLUDE ImmediateWIN64.inc
INCLUDE MemoryWIN64.inc
INCLUDE AdditionalWIN64.inc
INCLUDE NonBinaryWIN64.inc
INCLUDE UtilityWIN64.inc

EXTERN compare : PROC
EXTERN compare_add : PROC
EXTERN compare_and : PROC
EXTERN memory_read : PROC
EXTERN memory_write : PROC
EXTERN memory_read_pair : PROC
EXTERN memory_write_pair : PROC

BL_OPC:
    ; Saving the return address in the lr register
    ; Jumping to the subroutine
    ADD_REGISTER CYCLE_INDEX, 2

    MOV R10, RCX
    SHR R10, 6

    SIGN_EXTEND R11, R10, 26
    SAL R11, 2

    GET_REGISTER R10D, PC_INDEX
    SET_REGISTER_NO_ZERO R10D, LR_INDEX

    ADD_REGISTER R11D, PC_INDEX

	JMP CONTINUE_ADDRESS
BRANCH_OPC:
    MOV R10, RCX
    SHR R10, 6

    SIGN_EXTEND R11, R10, 26
    SAL R11, 2
    
    ADD_REGISTER R11D, PC_INDEX

    JMP CONTINUE_ADDRESS
SWI_OPC:
    JMP CONTINUE_ADDRESS
BRANCH_COND_OPC:
    ; Cond
    MOV R12, RCX
    SHR R12, 6h
    AND R12, 15

    ; Disp
    MOV R11, RCX
    SHR R11, 10

	SIGN_EXTEND R10D, R11D, 22
	SAL R10, 2

    GET_REGISTER R11D, FLAGS_INDEX

    LEA RAX, BRANCH_CONDITIONAL_OPCODE
    MOV RBX, QWORD PTR[RAX+R12*8]
    JMP RBX

    JMP CONTINUE_ADDRESS
LOGICAL_ADD_SUB_OPC:
    ; LogicalAddSub Opcode
    MOV RBX, RCX
    SHR RBX, 6h
    AND RBX, 3Fh

    ; Dest register
    MOV R10, RCX
    SHR R10, 12
    AND R10, 1Fh

    ; First Source
    MOV R11, RCX
    SHR R11, 17
    AND R11, 1Fh

    ; Second Source
    MOV R12, RCX
    SHR R12, 22
    AND R12, 1Fh

    ; Third Source
    MOV R13, RCX
    SHR R13, 27

    LEA RAX, LOGICAL_ADD_SUB_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX

    JMP CONTINUE_ADDRESS
FBINARY_OPC:
    JMP CONTINUE_ADDRESS
MEMORY_IMMEDIATE_OPC:
    ; Memory opcode
    MOV RBX, RCX
    SHR RBX, 6
    AND RBX, 7h

    ; Dest register
    MOV R10, RCX
    SHR R10, 9
    AND R10, 1Fh
    
    ; Base register
    MOV R11, RCX
    SHR R11, 14
    AND R11, 1Fh

    ; Disp
    MOV R13, RCX
    SHR R13, 19
    AND R13, 1

    MOV R14, RCX
    SHR R14, 20
    
    CMP R13, 1
    JNZ MEMORY_SKI_SUB
    NEG R14
MEMORY_SKI_SUB:

    LEA RAX, MEMORY_IMMEDIATE_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX

    JMP CONTINUE_ADDRESS
FMEMORY_IMMEDIATE_OPC:
    ; Memory opcode
    MOV RBX, RCX
    SHR RBX, 6
    AND RBX, 7h

    ; Dest register
    MOV R10, RCX
    SHR R10, 9
    AND R10, 1Fh
    
    ; Base register
    MOV R11, RCX
    SHR R11, 14
    AND R11, 1Fh

    ; Disp
    MOV R13, RCX
    SHR R13, 19
    AND R13, 1

    MOV R14, RCX
    SHR R14, 20
    
    CMP R13, 1
    JNZ FMEMORY_SKI_SUB
    NEG R14
FMEMORY_SKI_SUB:

    LEA RAX, FMEMORY_IMMEDIATE_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX

    JMP CONTINUE_ADDRESS
MEMORY_PAIR_OPC:
    ; Memory Opcode
    MOV RBX, RCX
    SHR RBX, 6
    AND RBX, 7h

    ; Dest register
    MOV R10, RCX
    SHR R10, 9
    AND R10, 1Fh
    
    ; Second Dest/Source register
    MOV R11, RCX
    SHR R11, 14
    AND R11, 1Fh

    ; Base Register
    MOV R12, RCX
    SHR R12, 19
    AND R12, 1Fh

    ; Disp
    MOV R13, RCX
    SHR R13, 24
    AND R13, 1h

    MOV R14, RCX
    SHR R14, 25
    
    TEST R13, R13
    JZ MEMORY_PAIR_SKI_SUB
    NEG R14
MEMORY_PAIR_SKI_SUB:

    LEA RAX, MEMORY_PAIR_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX

    JMP CONTINUE_ADDRESS
ADDITIONAL_OPC:
    ; Additional Opcode
    MOV RBX, RCX
    SHR RBX, 6
    AND RBX, 3Fh

    ; Dest register
    MOV R10, RCX
    SHR R10, 12h
    AND R10, 1Fh

    ; First Source
    MOV R11, RCX
    SHR R11, 17
    AND R11, 1Fh

    ; Second Source
    MOV R12, RCX
    SHR R12, 22
    AND R12, 1Fh

    ; Third Source
    MOV R13, RCX
    SHR R13, 27
    AND R13, 1Fh

    LEA RAX, ADDITIONAL_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX
NON_BINARY_OPC:
    ; Non Binary Opcode
    MOV RBX, RCX
    SHR RBX, 6
    AND RBX, 3Fh

    ; Dest register
    MOV R11, RCX
    SHR R11, 12h
    AND R11, 1Fh

    ; First Source
    MOV R12, RCX
    SHR R12, 17
    AND R12, 1Fh

    ; Second Source
    MOV R13, RCX
    SHR R13, 22
    AND R13, 1Fh

    ; Third Source
    MOV R14, RCX
    SHR R14, 27

    LEA RAX, NON_BINARY_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX
LD_PC_OPC:
    ; Dest register
    MOV R10, RCX
    SHL R10, 6
    AND R10, 1Fh

    ; Disp21
    MOV R11D, EDX
    SHR R11D, 11

    SIGN_EXTEND EDX, R11D, 21
    SAL EDX, 2
    GET_REGISTER R13D, PC_INDEX
    ADD EDX, R13D
    
    MOV ECX, R10D
    MOV R8, 0
    CALL memory_read

    RESTORE_ARGS

    MOV EAX, DWORD PTR[RAX]
    SET_REGISTER EAX, R10
    
    JMP CONTINUE_ADDRESS
LD_S_PC_OPC:
    JMP CONTINUE_ADDRESS
LD_D_PC_OPC:
    JMP CONTINUE_ADDRESS
LD_Q_PC_OPC:
    JMP CONTINUE_ADDRESS
ADR_PC_OPC:
    ; Dest register
    MOV R10, RCX
    SHL R10, 6
    AND R10, 1Fh

    ; Disp21
    MOV R11, RCX
    SHR R11, 11

    SIGN_EXTEND R12D, R11D, 21
    SAL R12D, 2

    GET_REGISTER R13D, PC_INDEX
    ADD R13D, R12D
    SET_REGISTER R13D, R10

    JMP CONTINUE_ADDRESS
IMMEDIATE_OPC:
    ; Immediate opcode
    MOV RBX, RCX
    SHR RBX, 6
    AND RBX, 1Fh

    ; Dest register
    MOV R10, RCX
    SHR R10, 11
    AND R10, 1Fh

    ; Immediate value
    MOV R11, RCX
    SHR R11, 10h

    LEA RAX, IMMEDIATE_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX
ADD_IMMEDIATE_OPC:
    IMMEDIATE_16_PROLOGUE
    
    GET_REGISTER R11D, R11
    ADD R11D, R12D
    SET_REGISTER R11D, R10

    JMP CONTINUE_ADDRESS
ADDS_IMMEDIATE_OPC:
    IMMEDIATE_16_PROLOGUE

    GET_REGISTER ECX, R11
    MOV EDX, R12D
    CALL compare_add

    RESTORE_ARGS
    SET_REGISTER EAX, R10

    JMP CONTINUE_ADDRESS
SUB_IMMEDIATE_OPC:
    IMMEDIATE_16_PROLOGUE

    GET_REGISTER R13D, R11
    SUB R13D, R12D
    SET_REGISTER R13D, R10

    JMP CONTINUE_ADDRESS
SUBS_IMMEDIATE_OPC:
    IMMEDIATE_16_PROLOGUE

    GET_REGISTER ECX, R11
    MOV RDX, R12
    CALL compare

    RESTORE_ARGS
    SET_REGISTER EAX, R10

    JMP CONTINUE_ADDRESS
AND_IMMEDIATE_OPC:
    IMMEDIATE_16_PROLOGUE

    GET_REGISTER R11D, R11
    AND R11D, R12D
    SET_REGISTER R11D, R10

    JMP CONTINUE_ADDRESS
ANDS_IMMEDIATE_OPC:
    IMMEDIATE_16_PROLOGUE

    GET_REGISTER ECX, R11
    MOV EDX, R12D
    CALL compare_and

    RESTORE_ARGS
    SET_REGISTER EAX, R10

    JMP CONTINUE_ADDRESS
OR_IMMEDIATE_OPC:
    IMMEDIATE_16_PROLOGUE

    GET_REGISTER R11D, R11
    OR R11D, R12D
    SET_REGISTER R11D, R10
    
    JMP CONTINUE_ADDRESS
ORN_IMMEDIATE_OPC:
    IMMEDIATE_16_PROLOGUE

    GET_REGISTER R11D, R11
    NOT R12D
    OR R11D, R12D
    SET_REGISTER R11D, R10

    JMP CONTINUE_ADDRESS
EOR_IMMEDIATE_OPC:
    IMMEDIATE_16_PROLOGUE

    GET_REGISTER R11D, R11
    XOR R11D, R12D
    SET_REGISTER R11D, R10

    JMP CONTINUE_ADDRESS
TBZ_OPC:
    JMP CONTINUE_ADDRESS
TBNZ_OPC:
    JMP CONTINUE_ADDRESS
CBZ_OPC:
    ; Dest register
    MOV R10, RCX
    SHL R10, 6
    AND R10, 1Fh

    ; Disp21
    MOV R11, RCX
    SHR R11, 11

    SIGN_EXTEND R12D, R11D, 21
    SAL R12D, 2

    ; We make the comparision to set the flags
    GET_REGISTER ECX, R10
    MOV EDX, 0

    CALL compare

    RESTORE_ARGS
    
    ; We dont check the flags because it's too slow
    CMP R10D, 0
    JNZ CBZ_SKIP
    ADD_REGISTER R12D, PC_INDEX

CBZ_SKIP:
    JMP CONTINUE_ADDRESS
CBNZ_OPC:
    ; Dest register
    MOV R10, RCX
    SHL R10, 6
    AND R10, 1Fh

    ; Disp21
    MOV R11, RCX
    SHR R11, 11

    SIGN_EXTEND R12D, R11D, 21
    SAL R12D, 2

    ; We make the comparision to set the flags
    GET_REGISTER ECX, R10
    MOV EDX, 0

    CALL compare

    RESTORE_ARGS
    
    ; We dont check the flags because it's too slow
    CMP R10D, 0
    JZ CBNZ_SKIP
    ADD_REGISTER R12D, PC_INDEX

CBNZ_SKIP:
    JMP CONTINUE_ADDRESS

; --------------------------------------------
;               MAIN INTERPRETER
; --------------------------------------------

; RCX = Instruction
; RDX = Register List
; R8 = PC Pointer
InterpreterMain PROC
    MOV QWORD PTR[RSP+8], RCX
    MOV QWORD PTR[RSP+16], RDX
    MOV QWORD PTR[RSP+24], R8
    MOV QWORD PTR[RSP+32], R9

    SUB RSP, 30h

	; Interpreter zone
	MOV RBX, RCX
	AND RBX, 3Fh

    ; One cycle for decoding and simple operations
    ADD_REGISTER 1, CYCLE_INDEX

	LEA RAX, MAIN_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX
CONTINUE:
CONTINUE_ADDRESS EQU $

    ADD RSP, 30h
	RET
InterpreterMain ENDP

END