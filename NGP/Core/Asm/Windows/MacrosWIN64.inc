; --------------------------------------------
;                   MACROS
; --------------------------------------------

SAVE_ARGS MACRO
    MOV QWORD PTR[RSP+30h+8], RCX
    MOV QWORD PTR[RSP+30h+16], RDX
    MOV QWORD PTR[RSP+30h+24], R8
    MOV QWORD PTR[RSP+30h+32], R9
ENDM

RESTORE_ARGS MACRO
    MOV RCX, QWORD PTR[RSP+30h+8]
    MOV RDX, QWORD PTR[RSP+30h+16]
    MOV R8, QWORD PTR[RSP+30h+24]
    MOV R9, QWORD PTR[RSP+30h+32]
ENDM

IMMEDIATE_16_PROLOGUE MACRO
    ; Dest register
    MOV R10, RCX
    SHR R10, 6h
    AND R10, 1Fh
    
     ; Source register
    MOV R11, RCX
    SHR R11, 11
    AND R11, 1Fh

    ; Immediate value
    MOV R12, RCX
    SHR R12, 10h
ENDM

SHIFT_REG MACRO dest_src1, src2, Function
    MOV QWORD PTR[RSP], RCX
    MOV QWORD PTR[RSP+8], RDX

    MOV RCX, dest_src1
    MOV RDX, src2
    CALL Function
    MOV dest_src1, RAX
    
    MOV RCX, QWORD PTR[RSP]
    MOV RDX, QWORD PTR[RSP+8]
ENDM


BINARY_OPERATION MACRO Op, ShiftFunction
    MOV R14D, DWORD PTR[RDX+R11*4]
    MOV R15D, DWORD PTR[RDX+R12*4]
    SHIFT_REG R15, R13, ShiftFunction
    
    OP R14D, R15D
    MOV DWORD PTR [RDX+R10*4], R14D
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
