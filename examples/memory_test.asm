; Test memory operations
LDI R0, 100
ST R0, [0x0100]
LD R1, [0x0100]

; Test stack operations  
LDI R0, 42
PUSH R0
POP R1

; Test multiple stack operations
LDI R0, 10
LDI R1, 20
LDI R2, 30
PUSH R0
PUSH R1  
PUSH R2
POP R0
POP R1
POP R2

HLT