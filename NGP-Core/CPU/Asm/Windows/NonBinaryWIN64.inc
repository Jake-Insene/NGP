; --------------------------------------------
;         NON BINARY INSTRUCTIONS
; --------------------------------------------
; RDX = Register List
; R10 = Dest Index
; R11 = First Source
; R12 = Second Source
; R13 = Third Source

RET_OPC:
    GET_REGISTER R10D, R10
    SET_REGISTER_NO_ZERO R10D, PC_INDEX

    JMP CONTINUE_ADDRESS
HALT_OPC:
    SET_REGISTER_NO_ZERO 1, HALT_INDEX
    JMP CONTINUE_ADDRESS