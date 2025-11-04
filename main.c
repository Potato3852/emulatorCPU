#include "../core/cpu.h"
#include "../assembler/assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file.asm>\n", argv[0]);
        return 1;
    }
    
    // Open file
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* program = malloc(size + 1);
    fread(program, 1, size, file);
    program[size] = '\0';
    fclose(file);

    // Run program
        
    struct CPU cpu;
    cpu_init(&cpu);

    if(argc > 2) {
        if(strcmp(argv[2], "--debug") == 0) {
            cpu.debugger.interactive = 1;
        } else if(strcmp(argv[2], "--trace") == 0) {
            cpu.debugger.trace_mode = 1;
        }
    }
    
    assemble(&cpu, program, 0x0000);
    free(program);

    cpu.PC = 0x0000;
    cpu_run(&cpu);
    cpu_print_state(&cpu);

    printf("\n=== Memory dump around ISR0 (0x000E) ===\n");
    for(int i = 0x000E; i < 0x0015; i++) {
        printf("0x%04X: 0x%02X\n", i, cpu.memory[i]);
    }
        
    return 0;
}