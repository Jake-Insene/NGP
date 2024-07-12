; --------------------------------------------
;           ADDITIONAL INSTRUCTIONS
; --------------------------------------------
; RDX = Register List
; R10 = Dest Index
; R11 = First Source
; R12 = Second Source
; R13 = Third Source

INCLUDE MacrosWIN64.inc

LD_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 0
    CALL memory_read

    RESTORE_ARGS
    
    JMP CONTINUE_ADDRESS
LDSH_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 2
    CALL memory_read

    RESTORE_ARGS

    JMP CONTINUE_ADDRESS
LDH_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 1
    CALL memory_read

    RESTORE_ARGS

    JMP CONTINUE_ADDRESS
LDSB_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 4
    CALL memory_read

    RESTORE_ARGS

    JMP CONTINUE_ADDRESS
LDB_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 3
    CALL memory_read

    RESTORE_ARGS

    JMP CONTINUE_ADDRESS
ST_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 0
    CALL memory_write

    RESTORE_ARGS

    JMP CONTINUE_ADDRESS
STH_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 1
    CALL memory_write

    RESTORE_ARGS

    JMP CONTINUE_ADDRESS
STB_OPC:
    GET_REGISTER EDX, R11
    GET_REGISTER R12D, R12
    ADD EDX, R12D

    MOV ECX, R10D
    MOV R8, 3
    CALL memory_write

    RESTORE_ARGS

    JMP CONTINUE_ADDRESS
MADD_OPC:
    JMP CONTINUE_ADDRESS
MSUB_OPC:
    JMP CONTINUE_ADDRESS
UMADD_OPC:
    JMP CONTINUE_ADDRESS
UMSUB_OPC:
    JMP CONTINUE_ADDRESS
DIV_OPC:
    JMP CONTINUE_ADDRESS
UDIV_OPC:
    JMP CONTINUE_ADDRESS
SHL_OPC:
    GET_REGISTER R11D, R11
    GET_REGISTER R12D, R12
    SHIFT_REG R11D, R12D, SHL
    SET_REGISTER R11D, R10

    JMP CONTINUE_ADDRESS
SHR_OPC:
    GET_REGISTER R11D, R11
    GET_REGISTER R12D, R12
    SHIFT_REG R11D, R12D, SHR
    SET_REGISTER R11D, R10

    JMP CONTINUE_ADDRESS
ASR_OPC:
    GET_REGISTER R11D, R11
    GET_REGISTER R12D, R12
    SHIFT_REG R11D, R12D, SAR
    SET_REGISTER R11D, R10

    JMP CONTINUE_ADDRESS
ROR_OPC:
    GET_REGISTER R11D, R11
    GET_REGISTER R12D, R12
    SHIFT_REG R11D, R12D, ROR
    SET_REGISTER R11D, R10

    JMP CONTINUE_ADDRESS
ABS_OPC:
    JMP CONTINUE_ADDRESS