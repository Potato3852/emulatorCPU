# Instruction Set

## Data Transfer
- `LDI R0, 10` - Load immediate
- `MOV R1, R0` - Move between registers

## Arithmetic
- `ADD R0, R1` - Addition
- `SUB R0, R1` - Subtraction  
- `INC R0`     - Increment

## Control Flow
- `JMP label` - Jump unconditional
- `JZ label`  - Jump if zero
- `JNZ label` - Jump if not zero
- `HLT`       - Halt execution

## Comparison
- `CMP R0, R1` - Compare registers

## Memory Operations
- `ST R0, [address]` - Store register to memory
- `LD R1, [address]` - Load from memory to register
- `PUSH R0` - Push register to stack
- `POP R1` - Pop from stack to register