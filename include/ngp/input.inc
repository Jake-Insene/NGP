; Input Registers
INPUT 		        =    0x00 ; 0x03000000...0x03000003 Input Status		    4B/R

INPUT_L1	        =    0x01 ; INPUT: L1					                    Bit 0
INPUT_L2    	    =    0x02 ; INPUT: L2					                    Bit 1
INPUT_R1	        =    0x04 ; INPUT: R1					                    Bit 2
INPUT_R2	        =    0x08 ; INPUT: R2					                    Bit 3

INPUT_START 	    =    0x10 ; INPUT: Button Start				                Bit 4
INPUT_SELECT	    =    0x20 ; INPUT: Button Select				            Bit 5

INPUT_X 	        =    0x40 ; INPUT: Button X				                    Bit 6
INPUT_Y 	        =    0x80 ; INPUT: Button Y				                    Bit 7
INPUT_Z 	        =   0x100 ; INPUT: Button Z				                    Bit 8
INPUT_W             =   0x200 ; INPUT: Button W                                 Bit 9

INPUT_L3            =  0x4000 ; INPUT: L3                                       Bit 10
INPUT_R3            = 0x80000 ; INPUT: R3                                       Bit 11
