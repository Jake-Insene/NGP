; Privilage Modes
PRIVILAGE_MODE = 0x10 ; 0x03000010...0x03000013 Privilage Mode                  4B/R

; Kernel Mode: You have access to all the address space, 
; any exception produced here will shutdown the console
PRIVILAGE_MODE_KERNEL   = 0x00

; User Mode: You dont have access to all the address space,
; A game must to be in User mode always, a exception produced
; here it will be send to the BIOS RUTINE
PRIVILAGE_MODE_USER     = 0x01
