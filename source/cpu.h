#ifndef CPU_H
#define CPU_H

#include<stdint.h>
#include<stdio.h>

#define MEMORY_SIZE 65536
#define FLAG_ZERO (1 << 0)
#define FLAG_CARRY (1 << 1)

//Opcodes
#define OP_HLT 0x00
#define OP_LDI 0x01
#define OP_ADD 0x02
#define OP_SUB 0x03
#define OP_JMP 0x04
#define OP_JZ  0x05
#define OP_MOV 0x06
#define OP_INC 0x07
#define OP_CMP 0x08
#define OP_JNZ 0x09

struct CPU {
    uint8_t R[4];                   // General purpose registers
    uint16_t PC;                    // PC registers
    uint8_t FLAGS;                  // Status flags
    uint8_t running;                // CPU running status
    uint8_t memory[MEMORY_SIZE];    // Main memory
};

// Rules for registers:
// R0 - Main register for results and function return values
// R1-R3 - Additional arguments and temporary data

void cpu_init(struct CPU* cpu);
void cpu_print_state(struct CPU* cpu);
void memory_write(struct CPU* cpu, uint16_t address, uint8_t value);
uint8_t memory_read(struct CPU* cpu, uint16_t address);
void cpu_step(struct CPU* cpu);
void cpu_run(struct CPU* cpu);
void update_flags(struct CPU* cpu, uint8_t result);

#endif