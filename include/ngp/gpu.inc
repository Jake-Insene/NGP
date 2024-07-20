; GPU Constants
; The total number of command buffers
GPU_COMMAND_BUFFER_COUNT = 4

; GPU Registers
GPU0 	                = 0x00 ; 0x03000004...0x03000007 GPU Command Buffer     4B/W
GPU1 	                = 0x04 ; 0x03000008...0x0300000B GPU Display            4B/W

; GPU Command buffers
GPU_COMMAND_BUFFER0         = 0x00
GPU_COMMAND_BUFFER1         = 0x01
GPU_COMMAND_BUFFER2         = 0x02
GPU_COMMAND_BUFFER3         = 0x03

; GPU Commands
GPU_COMMAND_BEGIN       = 0x00 ; Command: Begin buffer record
GPU_COMMAND_END         = 0x01 ; Command: End buffer record
GPU_COMMAND_EXECUTE     = 0x02 ; Command: Execute the current recorded buffer

; WARN: You dont must to use the following commands between COMMAND_BEGIN and
; COMMAND_END

; Command: Set current command buffer
; The command has the following encoding
; C = 0x03
; B = Buffer
; 0xCC0000BB
GPU_COMMAND_SET_BUFFER  = 0x03

; WARN: You must to use the following commands between COMMAND_BEGIN and
; COMMAND_END

; Command: Fill a rectangle
; The command requires the following word arguments
; COLOR:    0xAABBGGRR
; POSITION: 0xYYYYXXXX
; SIZE:     0xWWWWHHHH
GPU_COMMAND_FILL_RECT   = 0x04

