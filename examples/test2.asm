; Test interrupt system - FINAL FIXED version
START:
    LDI R0, 100        ; 0x0000
    ST R0, [0x0100]    ; 0x0003  
    EI                 ; 0x0007
    
    ; Set up interrupt vector 0 to point to ISR0 (0x001C)
    LDI R0, 0x1C       ; 0x0008: Low byte of ISR0 address
    LDI R1, 0x00       ; 0x000B: High byte  
    ST R0, [0x0000]    ; 0x000E
    ST R1, [0x0001]    ; 0x0012
    
    INT 0              ; 0x0016: This should jump to 0x001C
    LDI R1, 200        ; 0x0018: This executes after IRET
    HLT                ; 0x001B

; Interrupt handler starts at 0x001C
ISR0:
    PUSH R0            ; 0x001C
    PUSH R1            ; 0x001E
    
    LDI R0, 255        ; 0x0020
    MOV R2, R0         ; 0x0023
    
    POP R1             ; 0x0026
    POP R0             ; 0x0028
    
    IRET               ; 0x002A