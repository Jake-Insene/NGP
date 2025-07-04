; [Bit Index/Bit Range] Name/Info, Values
; Name = Address

IO_BASE = 0x10000000

IRQ_BASE =		IO_BASE | 0x00000
DMA_BASE =		IO_BASE | 0x01000
PAD_BASE =		IO_BASE | 0x02000
USI_BASE =		IO_BASE | 0x03000
DISPLAY_BASE =	IO_BASE | 0x04000
GU_BASE =       IO_BASE | 0x10000

; IRQ Registers

; [0] DMA
; [1] TIMER
; [2] SPU
; [3] USI
; [4] GU
; [5] DISPLAY
; [31] PAD
IRQ_MASK =		IRQ_BASE | 0x00
IRQ_STATUS =	IRQ_BASE | 0x04


; DMA
DMA_RAM_CHANNEL =	DMA_BASE | 0x000
DMA_USI_CHANNEL =	DMA_BASE | 0x010
DMA_SPU_CHANNEL =	DMA_BASE | 0x020
DMA_GU_CHANNEL =	DMA_BASE | 0x030

; [0] Start / Busy
; [1 - 2] Priority
; [3 - 4] Transfer direction:
;         0(RAM -> DEVICE), 1(DEVICE -> RAM), 2(DEVICE -> DEVICE)
; [5 - 7] Transfer Step:
;         0(4 bytes/1 Word), 1(2 bytes/1 Half), 2(1 byte), 3(8 bytes/1 DWord),
;         4(16 bytes/1 QWord)
;
; [30] Fill Mode: 0(Normal fill mode, same as a copy from dest -> src),
;                 1(Fill dest memory range with the value in the src register)
; [31] IRQ Enable

; DMA Channel Register Src
; [0 - 31] Source Address

; DMA Channel Register Dest
; [0 - 31] Destination Address

; DMA Channel Register Cnt
; [0 - 31] Word Count

DMA_RAM_CHANNEL_CTR = DMA_RAM_CHANNEL | 0x0
DMA_RAM_CHANNEL_DST = DMA_RAM_CHANNEL | 0x4
DMA_RAM_CHANNEL_SRC = DMA_RAM_CHANNEL | 0x8
DMA_RAM_CHANNEL_CNT = DMA_RAM_CHANNEL | 0xC

DMA_EMD_CHANNEL_CTR = DMA_USI_CHANNEL | 0x0
DMA_EMD_CHANNEL_DST = DMA_USI_CHANNEL | 0x4
DMA_EMD_CHANNEL_SRC = DMA_USI_CHANNEL | 0x8
DMA_EMD_CHANNEL_CNT = DMA_USI_CHANNEL | 0xC

DMA_SPU_CHANNEL_CTR = DMA_SPU_CHANNEL | 0x0
DMA_SPU_CHANNEL_DST = DMA_SPU_CHANNEL | 0x4
DMA_SPU_CHANNEL_SRC = DMA_SPU_CHANNEL | 0x8
DMA_SPU_CHANNEL_CNT = DMA_SPU_CHANNEL | 0xC

DMA_GU_CHANNEL_CTR = DMA_GU_CHANNEL | 0x0
DMA_GU_CHANNEL_DST = DMA_GU_CHANNEL | 0x4
DMA_GU_CHANNEL_SRC = DMA_GU_CHANNEL | 0x8
DMA_GU_CHANNEL_CNT = DMA_GU_CHANNEL | 0xC

; Channel Interrupt Mask
; [0] RAM
; [1] EMD
; [2] SPU
; [2] GU
DMA_IRQ_MASK =		DMA_BASE | 0x100
DMA_IRQ_STATUS =	DMA_BASE | 0x104
DMA_WAIT_ON_MASK =	DMA_BASE | 0x108


; PAD
IO_PAD_MAIN_BUTTONS =	PAD_BASE | 0x000
IO_PAD_MAIN_STICKS =	PAD_BASE | 0x004
IO_PAD_MAIN_STATUS =	PAD_BASE | 0x008


; Display Registers

; Display Interrupt Mask
; [0] HBLANK
; [1] VBLANK
DISPLAY_IRQ_MASK =		DISPLAY_BASE | 0x000
DISPLAY_IRQ_STATUS =	DISPLAY_BASE | 0x004

; Display Controller
; [0] Enable
; [1] Present
DISPLAY_CTR =			DISPLAY_BASE | 0x008

; Display Model ID
DISPLAY_ID =		DISPLAY_BASE | 0x00C

; Display Buffer Address
DISPLAY_BUFFER =	DISPLAY_BASE | 0x010
; [0 - 13] Width
; [14 - 27] Height
; [28 - 31] Display Format
DISPLAY_FORMAT =	DISPLAY_BASE | 0x014


; GU Registers
; GU Interrupt Mask
; [0] Queue
GU_IRQ_MASK =	GU_BASE | 0x0000
GU_IRQ_STATUS = GU_BASE | 0x0004
; [0] Restart GU : 1 -> Restart
GU_CTR =		GU_BASE | 0x0008
; [0 - 31] GU Model ID
GU_ID =			GU_BASE | 0x000C

; Queue Controller
; [31] Start Execution
GU_QUEUE_CTR =		GU_BASE | 0x0010
; Queue State
; [0 - 31] Queue State
GU_QUEUE_STATUS =	GU_BASE | 0x0014

; Queue Command List Address
; [0 - 31] Command List Base Address
GU_QUEUE_ADDR =		GU_BASE | 0x0018
; Queue Command Buffer Length
; [0 - 31] Buffer Length
GU_QUEUE_LEN =		GU_BASE | 0x001C

