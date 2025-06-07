# NGPv1 ABI

## NGPv1 Registers
 * R0-R28: General purpose registers.
 * SP: Stack pointer register.
 * LR: Link register.
 * ZR: Zero register (always zero).

## Use of Registers
 * R0: Used for function return values.
 * R0-R28: Used for passing arguments to functions and temporary storage.
 * SP: Points to the top of the stack.
 * LR: Stores the return address for function calls.
 * ZR: Used for zeroing out registers or comparisons.