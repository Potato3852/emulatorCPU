# Assembly Language Syntax

## Basic Syntax
Each line contains either:
- An instruction
- A label definition
- A comment

## Instructions Format
[label:] OPCODE operand1, operand2 [; comment]

## Operand Types

### Registers
- `R0`, `R1`, `R2`, `R3` - General purpose registers

### Immediate Values
- Decimal: `10`, `255`
- Hexadecimal: `0x0A`, `0xFF`

### Labels
- Defined with colon: `my_label:`
- Used as jump targets: `JMP my_label`

## Instruction Reference

### Data Transfer
```asm
LDI R0, 100     ; Load immediate value 100 into R0
MOV R1, R0      ; Copy value from R0 to R1

### Arithmetic Operations
```asm
ADD R0, R1      ; R0 = R0 + R1
SUB R0, R1      ; R0 = R0 - R1  
INC R0          ; R0 = R0 + 1

### Comparison
CMP R0, R1      ; Compare R0 and R1, set flags

### Control Flow
```asm
my_label:       ; Label definition
JMP my_label    ; Unconditional jump
JZ  target      ; Jump if Zero flag set
JNZ target      ; Jump if Zero flag not set
HLT             ; Halt execution

### Comments
Start with semicolon: ; This is a comment
Can be at end of line: ADD R0, R1 ; Add values