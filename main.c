#include "core/cpu.h"
#include "assembler/assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("USAGE: %s <file.asm> [--debug|--trace]\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("ERROR: Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* program = malloc(size + 1);
    fread(program, 1, size, file);
    program[size] = '\0';
    fclose(file);

    struct CPU* cpu = malloc(sizeof(struct CPU));
    if (!cpu) {
        printf("FATAL: Cannot allocate CPU memory!\n");
        free(program);
        return 1;
    }
    cpu_init(cpu);

    if (argc > 2) {
        if (strcmp(argv[2], "--debug") == 0) {
            cpu->debugger.interactive = 1;
        } else if (strcmp(argv[2], "--trace") == 0) {
            cpu->debugger.trace_mode = 1;
        }
    }
    
    assemble(cpu, program, 0x0010);

    cpu->PC = 0x0010;

    memory_write(cpu, 0x0002, 0x2F);
    memory_write(cpu, 0x0003, 0x00);

    printf("Starting execution at 0x%04X\n", cpu->PC);

    cpu_run(cpu);

    printf("=== Execution complete ===\n");

    free(program);
    free(cpu);
        
    return 0;
}