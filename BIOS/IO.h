; [Bit Index/Bit Range] Name/Info, Values
; Name = Address

IO_BASE = 0x10000000

IRQ_BASE =		IO_BASE | 0x00000
DMA_BASE =		IO_BASE | 0x01000
PAD_BASE =		IO_BASE | 0x02000
EMD_BASE =		IO_BASE | 0x03000
DISPLAY_BASE =	IO_BASE | 0x04000
GU_BASE =       IO_BASE | 0x10000

; IRQ Registers

; [0] DMA
; [1] TIMER
; [2] SPU
; [3] EMD
; [4] GU
; [5] DISPLAY
; [31] PAD
IRQ_MASK =		IRQ_BASE | 0x00
IRQ_STATUS =	IRQ_BASE | 0x04


; DMA
DMA_RAM_CHANNEL = DMA_BASE | 0x000
DMA_EMD_CHANNEL = DMA_BASE | 0x010
DMA_SPU_CHANNEL = DMA_BASE | 0x020

; DMA Channel Register Controller
; [0] Start / Busy
; [1 - 2] Priority Level
; [31] IRQ Enable

; DMA Channel Register Src
; [0 - 31] Source Address

; DMA Channel Register Dest
; [0 - 31] Destination Address

; DMA Channel Register Cnt
; [0 - 31] Word Count

DMA_RAM_CHANNEL_CTR = DMA_RAM_CHANNEL | 0x0
DMA_RAM_CHANNEL_SRC = DMA_RAM_CHANNEL | 0x4
DMA_RAM_CHANNEL_DST = DMA_RAM_CHANNEL | 0x8
DMA_RAM_CHANNEL_CNT = DMA_RAM_CHANNEL | 0xC

DMA_EMD_CHANNEL_CTR = DMA_EMD_CHANNEL | 0x0
DMA_EMD_CHANNEL_SRC = DMA_EMD_CHANNEL | 0x4
DMA_EMD_CHANNEL_DST = DMA_EMD_CHANNEL | 0x8
DMA_EMD_CHANNEL_CNT = DMA_EMD_CHANNEL | 0xC

DMA_SPU_CHANNEL_CTR = DMA_SPU_CHANNEL | 0x0
DMA_SPU_CHANNEL_SRC = DMA_SPU_CHANNEL | 0x4
DMA_SPU_CHANNEL_DST = DMA_SPU_CHANNEL | 0x8
DMA_SPU_CHANNEL_CNT = DMA_SPU_CHANNEL | 0xC

; Channel Interrupt Mask
; [0] RAM
; [1] EMD
; [2] SPU
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
DISPLAY_IRQ_MASK =		DISPLAY_BASE | 0x000
DISPLAY_IRQ_STATUS =	DISPLAY_BASE | 0x004

; Display Controller
; [0] Enable
DISPLAY_CTR =			DISPLAY_BASE | 0x008

; Display Model ID
DISPLAY_ID =			DISPLAY_BASE | 0x00C

; Display Buffer Address
DISPLAY_BUFFER =		DISPLAY_BASE | 0x010
; [0 - 13] Width
; [14 - 27] Height
; [28 - 31] Display Format
DISPLAY_FORMAT =		DISPLAY_BASE | 0x014

; Write anything to trigger a presentation
DISPLAY_PRESENT =		DISPLAY_BASE | 0x018


; GU Registers

; [0] Transfer
; [1] Queue
GU_IRQ_MASK =	GU_BASE | 0x0000
GU_IRQ_STATUS = GU_BASE | 0x0004
; [0] Restart GU : 1 -> Restart
GU_CTR =		GU_BASE | 0x0008
; [0 - 31] GU Model ID
GU_ID =			GU_BASE | 0x000C


; Transfer Controller
; VRAM Source Address
GU_TRANSFER_INADDR =	GU_BASE | 0x0010
; Destination Source Format
; [0 - 11] Width
; [12 - 23] Height
; [24 - 31] Display Format
GU_TRANSFER_INFMT =		GU_BASE | 0x0014
; VRAM Destination Address
GU_TRANSFER_OUTADDR =	GU_BASE | 0x0018
; [0 - 11] Width
; [12 - 23] Height
; [24 - 31] Display Format
GU_TRANSFER_OUTFMT =	GU_BASE | 0x001C

; [0 - 7] Transfer Command
; [31] Start Transfer
GU_TRANSFER_CTR =		GU_BASE | 0x0020
; [0] Transfer Complete
GU_TRANSFER_IRQ_MASK =	GU_BASE | 0x0024
; [31] Transfer Model ID
GU_TRANSFER_ID =		GU_BASE | 0x0028


; GPU Queue Controller
; [0 - 15] Command
; [16 - 17] Queue Index
GU_QUEUE_CMD =		GU_BASE | 0x0030
; Queue Command Buffer Address
; [0 - 31] Buffer Address
GU_QUEUE_ADDR =		GU_BASE | 0x0034
; Queue Controller
; [0] Queue 0 Execution State
; [1] Queue 1 Execution State
; [2] Queue 2 Execution State
; [3] Queue 3 Execution State
; [31] Start Execution
GU_QUEUE_CTR =		GU_BASE | 0x0038
GU_QUEUE_STATUS =	GU_BASE | 0x003C

