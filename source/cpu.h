#ifndef CPU_H
#define CPU_H

#include<stdint.h>
#include<stdio.h>

#define MEMORY_SIZE 65536
#define FLAG_ZERO (1 << 0)
#define FLAG_CARRY (1 << 1)
#define DEBUGGER_BREAKPOINTS 16

// Opcodes
#define OP_HLT  0x00
#define OP_LDI  0x01
#define OP_ADD  0x02
#define OP_SUB  0x03
#define OP_JMP  0x04
#define OP_JZ   0x05
#define OP_MOV  0x06
#define OP_INC  0x07
#define OP_CMP  0x08
#define OP_JNZ  0x09
#define OP_ST   0x0A
#define OP_LD   0x0B
#define OP_PUSH 0x0C
#define OP_POP  0X0D
#define OP_INT  0x0E
#define OP_IRET 0x0F
#define OP_EI   0x10
#define OP_DI   0x11

// Addresses
#define ADDR_DIRECT    0x00  // [0x1234]
#define ADDR_INDIRECT  0x01  // [R0]
#define ADDR_INDEXED   0x02  // [R0 + N]

struct Debugger {
    uint16_t breakpoints[DEBUGGER_BREAKPOINTS];
    int breakpoint_count;
    int trace_mode;
    int interactive;
    int steps;
    uint16_t last_pc;
};

struct CPU {
    uint8_t R[4];                   // General purpose registers
    uint16_t PC;                    // PC registers
    uint16_t SP;                    // Stack Pointer
    uint8_t FLAGS;                  // Status flags
    uint8_t running;                // CPU running status
    uint8_t memory[MEMORY_SIZE];    // Main memory
    struct Debugger debugger;       // Debugger

    uint8_t interrupt_enabled;      // Global interrupt enable flag
    uint8_t interrupt_pending;      // Pending interrupt number (0-7)
    uint16_t saved_PC;              // PC saved when interrupt occurs
    uint8_t saved_FLAGS;            // FLAGS saved when interrupt occurs
    uint8_t in_interrupt;           // Currently handling interrupt flag

    uint8_t addr_mode;              // Current addressing mode
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