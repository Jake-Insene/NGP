; --------------------------------------------
;                  UTILITY
; --------------------------------------------

ShiftLeftValue PROC
    MOV RAX, RCX

SHIFT_LOOP:
    TEST RDX, RDX
    JZ END_LOOP
    
    SHL RAX, 1
    DEC RDX
    JMP SHIFT_LOOP
END_LOOP:
    RET
ShiftLeftValue ENDP

ShiftRightValue PROC
    MOV RAX, RCX

SHIFT_LOOP:
    TEST RDX, RDX
    JZ END_LOOP
    
    SHR RAX, 1
    DEC RDX
    JMP SHIFT_LOOP
END_LOOP:
    RET
ShiftRightValue ENDP

AShiftRightValue PROC
    MOV RAX, RCX

SHIFT_LOOP:
    TEST RDX, RDX
    JZ END_LOOP
    
    SAR RAX, 1
    DEC RDX
    JMP SHIFT_LOOP
END_LOOP:
    RET
AShiftRightValue ENDP

RotateRightValue PROC
    MOV RAX, RCX

SHIFT_LOOP:
    TEST RDX, RDX
    JZ END_LOOP
    
    ROR RAX, 1
    DEC RDX
    JMP SHIFT_LOOP
END_LOOP:
    RET
RotateRightValue ENDP
