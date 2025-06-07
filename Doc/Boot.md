# NGP Boot Process

- [Boot]: BIOS is loaded into ram address 0x0000'0000 where the execution starts.
0x0000'0000: Is used as the default Vector Base Address

- The BIOS initializes the hardware and loads the bootloader from the disk into memory.
It expect's that the bootloader is in the first second of the disk, sector size is 4096 bytes for now.