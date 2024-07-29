.DATA

MAIN_OPCODE:
	dq BL_OPC
    dq B_OPC
    dq B_COND_OPC

    dq LOGICAL_ADD_SUB_OPC
    dq FP_OPC

    dq LOAD_STORE_IMMEDIATE_OPC
    dq LOAD_STORE_FP_IMMEDIATE_OPC
    dq LOAD_STORE_REGISTER_OPC
    dq LOAD_STORE_PAIR_OPC

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

B_CONDITIONAL_OPCODE:
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
    dq BAL_OPC
    dq BNV_OPC

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

FP_OPCODE:
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
    dq FNEG_S_OPC
    dq FNEG_D_OPC
    dq FABS_S_OPC
    dq FABS_D_OPC

LOAD_STORE_IMMEDIATE_OPCODE:
    dq LD_IMMEDIATE_OPC
    dq LDSH_IMMEDIATE_OPC
    dq LDH_IMMEDIATE_OPC
    dq LDSB_IMMEDIATE_OPC
    dq LDB_IMMEDIATE_OPC
    dq ST_IMMEDIATE_OPC
    dq STH_IMMEDIATE_OPC
    dq STB_IMMEDIATE_OPC

LOAD_STORE_FP_IMMEDIATE_OPCODE:
    dq LD_S_IMMEDIATE_OPC
    dq LD_D_IMMEDIATE_OPC
    dq LD_Q_IMMEDIATE_OPC
    dq ST_S_IMMEDIATE_OPC
    dq ST_D_IMMEDIATE_OPC
    dq ST_Q_IMMEDIATE_OPC

LOAD_STORE_REGISTER_OPCODE:
    dq LD_OPC
    dq LDSH_OPC
    dq LDH_OPC
    dq LDSB_OPC
    dq LDB_OPC
    dq ST_OPC
    dq STH_OPC
    dq STB_OPC
    dq LD_S_OPC
    dq LD_D_OPC
    dq LD_Q_OPC
    dq ST_S_OPC
    dq ST_D_OPC
    dq ST_Q_OPC

LOAD_STORE_PAIR_OPCODE:
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
    dq DIV_OPC
    dq UDIV_OPC
    dq SHL_OPC
    dq SHR_OPC
    dq ASR_OPC
    dq ROR_OPC
    dq ABS_OPC

NON_BINARY_OPCODE:
    dq RET_OPC
    dq BLR_OPC
    dq BR_OPC
    dq ERET_OPC
    dq BRK_OPC
    dq HLT_OPC
    dq SIT_OPC
    dq MSR_OPC
    dq MRS_OPC
    dq NOP_OPC

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
INCLUDE FPWIN64.inc
INCLUDE ImmediateWIN64.inc
INCLUDE LoadStoreWIN64.inc
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

    MOV R10, RCX
    SHR R10, 6

    SIGN_EXTEND R11, R10, 26
    SAL R11, 2

    GET_REGISTER R10D, PC_INDEX
    SET_REGISTER_NO_ZERO R10D, LR_INDEX

    ADD_REGISTER R11D, PC_INDEX

	JMP CONTINUE_ADDRESS
B_OPC:
    MOV R10, RCX
    SHR R10, 6

    SIGN_EXTEND R11, R10, 26
    SAL R11, 2
    
    ADD_REGISTER R11D, PC_INDEX

    JMP CONTINUE_ADDRESS
B_COND_OPC:
    ; Cond
    MOV R12, RCX
    SHR R12, 6
    AND R12, 15

    ; Disp
    MOV R11, RCX
    SHR R11, 10

	SIGN_EXTEND R10D, R11D, 22
	SAL R10, 2

    GET_REGISTER R11D, PSR_INDEX

    LEA RAX, B_CONDITIONAL_OPCODE
    MOV RBX, QWORD PTR[RAX+R12*8]
    JMP RBX
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
FP_OPC:
    JMP CONTINUE_ADDRESS
LOAD_STORE_IMMEDIATE_OPC:
    ; Load Store Immediate opcode
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

    LEA RAX, LOAD_STORE_IMMEDIATE_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX

    JMP CONTINUE_ADDRESS
LOAD_STORE_FP_IMMEDIATE_OPC:
    ; Load Store FP Immediate opcode
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
    LEA RAX, LOAD_STORE_FP_IMMEDIATE_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX
LOAD_STORE_REGISTER_OPC:
    ; Load Store Register Opcode
    MOV RBX, RCX
    SHR RBX, 6
    AND RBX, 7FFh

    ;Dest/Source register
    MOV R10, RCX
    SHR R10, 17
    AND R10, 3Fh
    
    ; Base register
    MOV R11, RCX
    SHR R11, 22
    AND R11, 1Fh

    ; Index register
    MOV R12, RCX
    SHR R12, 27
    AND R12, 1Fh

    LEA RAX, LOAD_STORE_REGISTER_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX
LOAD_STORE_PAIR_OPC:
    ; Load Store Pair Opcode
    MOV RBX, RCX
    SHR RBX, 6
    AND RBX, 7h

    ; Dest/Source register
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
    JZ LOAD_STORE_PAIR_SKI_SUB
    NEG R14
LOAD_STORE_PAIR_SKI_SUB:
    LEA RAX, LOAD_STORE_PAIR_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX
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

    ; Op
    MOV R10, RCX
    SHR R10, 16
    AND R10, 3Fh

    ; First Source
    MOV R11, RCX
    SHR R11, 22
    AND R11, 1Fh

    ; Second Source
    MOV R12, RCX
    SHR R12, 27
    AND R12, 1Fh

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
interpreter_main PROC
    MOV QWORD PTR[RSP+8], RCX
    MOV QWORD PTR[RSP+16], RDX
    MOV QWORD PTR[RSP+24], RBX

    SUB RSP, 30h
    MOV QWORD PTR[RSP], R10
    MOV QWORD PTR[RSP+8h], R11
    MOV QWORD PTR[RSP+10h], R12
    MOV QWORD PTR[RSP+18h], R13
    MOV QWORD PTR[RSP+20h], R14
    MOV QWORD PTR[RSP+28h], R15
    

	; Interpreter zone
	MOV RBX, RCX
	AND RBX, 3Fh

	LEA RAX, MAIN_OPCODE
	MOV RBX, QWORD PTR [RAX+RBX*8]
	JMP RBX
CONTINUE:
CONTINUE_ADDRESS EQU $

    MOV R10, QWORD PTR[RSP]
    MOV R11, QWORD PTR[RSP+8h]
    MOV R12, QWORD PTR[RSP+10h]
    MOV R13, QWORD PTR[RSP+18h]
    MOV R14, QWORD PTR[RSP+20h]
    MOV R15, QWORD PTR[RSP+28h]

    MOV RBX, QWORD PTR[RSP+30h+24] 

    ADD RSP, 30h
	RET
interpreter_main ENDP

END
