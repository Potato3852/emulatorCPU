#include "cpu.h"
#include "assembler.h"
#include <stdio.h>

int main() {
    struct CPU cpu;
    cpu_init(&cpu);
    
    const char* program = 
        "LDI R0, 0\n"
        "LDI R1, 1\n"
        "LDI R2, 6\n"
        "LDI R3, 1\n"
        "sum_loop:\n"
        "ADD R0, R1\n"
        "INC R1\n"
        "CMP R1, R2\n"
        "JNZ sum_loop\n"
        "HLT\n";
    
    assemble(&cpu, program, 0x0000);
    
    cpu.PC = 0x0000;
    cpu_run(&cpu);
    cpu_print_state(&cpu);
    
    return 0;
}