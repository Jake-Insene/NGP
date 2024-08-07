; --------------------------------------------
;            IMMEDIATE INSTRUCTIONS
; --------------------------------------------
; RDX = Register List
; R10 = Dest Index
; R11 = Immediate
MOVT_IMMEDIATE_OPC:
    SHL R11D, 16
    GET_REGISTER R12D, R10
    OR R12D, R11D
    SET_REGISTER R12D, R10
    JMP CONTINUE_ADDRESS
MVN_IMMEDIATE_OPC:
    NOT R11D
    SET_REGISTER R11D, R10
    JMP CONTINUE_ADDRESS
FMOV_S_IMMEDIATE_OPC:
    JMP CONTINUE_ADDRESS
FMOV_D_IMMEDIATE_OPC:
    JMP CONTINUE_ADDRESS