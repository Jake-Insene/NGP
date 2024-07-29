; --------------------------------------------
;                   MACROS
; --------------------------------------------

SP_INDEX = 29
LR_INDEX = 30
PC_INDEX = 32
IR_INDEX = 33
PSR_INDEX = 34
CYCLE_INDEX = 35
EL_RA0 = 36
EL_RA1 = 37

HALT_MASK = 40h

SAVE_ARGS MACRO
    MOV QWORD PTR[RSP+30h+8], RCX
    MOV QWORD PTR[RSP+30h+16], RDX
ENDM

RESTORE_ARGS MACRO
    MOV RCX, QWORD PTR[RSP+30h+8]
    MOV RDX, QWORD PTR[RSP+30h+16]
ENDM

GET_REGISTER MACRO Dest, Index
    MOV Dest, DWORD PTR[RDX+Index*4]
ENDM

GET_REGISTER_SIZE MACRO Dest, Index, Size
    MOV Dest, Size PTR[RDX+Index*4]
ENDM

SET_REGISTER MACRO Src, Index
    MOV R15D, 0
    MOV R14D, 1
    CMP Index, 31
    CMOVNE R15D, R14D
    ADD_REGISTER R15D, CYCLE_INDEX
    CMOVNE R15D, Src
    MOV DWORD PTR [RDX+Index*4], R15D
ENDM

SET_REGISTER_NO_ZERO MACRO Src, Index
    MOV DWORD PTR [RDX+Index*4], Src
ENDM

ADD_REGISTER MACRO Src, Index
    ADD DWORD PTR[RDX+Index*4], Src
ENDM

SUB_REGISTER MACRO Src, Index
    SUB DWORD PTR[RDX+Index*4], Src
ENDM

IMMEDIATE_16_PROLOGUE MACRO
    ; Dest register
    MOV R10D, ECX
    SHR R10D, 6h
    AND R10D, 1Fh
    
     ; Source register
    MOV R11D, ECX
    SHR R11D, 11
    AND R11D, 1Fh

    ; Immediate value
    MOV R12D, ECX
    SHR R12D, 10h
ENDM

SHIFT_REG MACRO Dest_Src1, Src2, SHIFT_TYPE
    MOV DWORD PTR[RSP], ECX
    MOV ECX, Src2

    SHIFT_TYPE Dest_Src1, CL

    MOV ECX, DWORD PTR[RSP]
ENDM


LOGICAL_ADD_SUB MACRO Op, SHIFT_FUNCTION
    GET_REGISTER R11D, R11
    GET_REGISTER R12D, R12
    SHIFT_REG R12D, R13D, SHIFT_FUNCTION
    
    OP R11D, R12D
    SET_REGISTER R11D, R10
ENDM

SIGN_EXTEND MACRO R1, Arg, Size
    MOV R1, Arg
    SAR R1, (Size - 1)     ; Shift arithmetic right by (input_size - 1) to isolate sign bit
    AND R1, 1             ; Extract sign bit

    ; Extend the sign bit to fill the higher bits
    NEG R1                ; Negate the sign bit
    SHL R1, (32 - Size)   ; Shift left to fill higher bits with sign bit

    OR R1, Arg
ENDM
