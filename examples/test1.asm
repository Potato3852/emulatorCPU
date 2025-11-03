; Test 2: Memory and stack operations
    LDI R0, 100
    ST R0, [0x0100]     ; Store 100 at address 0x0100
    
    LDI R0, 200
    ST R0, [0x0101]     ; Store 200 at address 0x0101
    
    LD R1, [0x0100]     ; R1 = 100
    LD R2, [0x0101]     ; R2 = 200
    
    ; Stack operations
    LDI R0, 42
    PUSH R0
    LDI R0, 84
    PUSH R0
    POP R1              ; R1 = 84
    POP R2              ; R2 = 42
    
    ; Multiple stack operations
    LDI R0, 1
    LDI R1, 2
    LDI R2, 3
    LDI R3, 4
    PUSH R0
    PUSH R1
    PUSH R2
    PUSH R3
    POP R0              ; R0 = 4
    POP R1              ; R1 = 3
    POP R2              ; R2 = 2
    POP R3              ; R3 = 1
    
    HLT