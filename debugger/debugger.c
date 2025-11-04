#include "../debugger/debugger.h"
#include <string.h>
#include <stdio.h>

void debugger_init(struct Debugger* dbg) {
    dbg->breakpoint_count = 0;
    dbg->trace_mode = 0;
    dbg->interactive = 0;
    dbg->steps = 0;
    dbg->last_pc = 0;

    for(int i = 0; i < DEBUGGER_BREAKPOINTS; i++) {
        dbg->breakpoints[i] = 0xFFFF;
    }
}

void debugger_add_breakpoint(struct Debugger* dbg, uint16_t address) {
    if(dbg->breakpoint_count < DEBUGGER_BREAKPOINTS) {
        dbg->breakpoints[dbg->breakpoint_count] = address;
        dbg->breakpoint_count++;
        printf("Breakpoint set at 0x%04X\n", address);
    } else {
        printf("Can't set breakpoint: maximum reached\n");
    }
}

void debugger_remove_breakpoint(struct Debugger* dbg, uint16_t address) {
    for(int i = 0; i < dbg->breakpoint_count; i++) {
        if(dbg->breakpoints[i] == address) {
            for(int j = 0; j < dbg->breakpoint_count - 1; j++) {
                dbg->breakpoints[j] = dbg->breakpoints[j+1];
            }
            dbg->breakpoint_count--;
            printf("Breakpoint removed at 0x%04X\n", address);
            return;
        }
    }
    printf("Breakpoint not found at 0x%04X\n", address);
}

void debugger_check_breakpoint(struct CPU* cpu) {
    for(int i = 0; i < cpu->debugger.breakpoint_count; i++) {
        if(cpu->PC == cpu->debugger.breakpoints[i]) {
            printf("=== BREAKPOINT at 0x%04X ===\n", cpu->PC);
            cpu->debugger.interactive = 1;
            return;
        }
    }
}

void debugger_interactive(struct CPU* cpu) {
    char command[32];

    while(1) {
        printf("(dbg) ");
        if(fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = 0;

        if(strcmp(command, "s") == 0) {
            cpu->debugger.interactive = 0;
            cpu_step(cpu);
            cpu->debugger.interactive = 1;
            debugger_print_registers(cpu);
            break;

        } else if(strcmp(command, "c") == 0) {
            cpu->debugger.interactive = 0;
            break;

        } else if(strcmp(command, "r") == 0) {
            debugger_print_registers(cpu);

        } else if(strncmp(command, "m ", 2) == 0) {
            uint16_t addr;
            if(sscanf(command + 2, "%hx", &addr) == 1) {
                debugger_print_memory(cpu, addr, 4);
            } else {
                printf("Usage: m <address>\n");
            }

        } else if(strncmp(command, "b ", 2) == 0) {
            uint16_t addr;
            if(sscanf(command + 2, "%hx", &addr) == 1) {
                debugger_add_breakpoint(&cpu->debugger, addr);
            } else {
                printf("Usage: b <address>\n");
            }

        } else if(strcmp(command, "q") == 0) {
            cpu->running = 0;
            break;
        } else if(strcmp(command, "help") == 0) {
            printf("Commands: s=step, c=continue, r=registers, m=memory, b=breakpoint, q=quit\n");
        } else {
            printf("Unknown command. Type 'help' for commands.\n");
        }
    }
}

void debugger_print_registers(struct CPU* cpu) {
    printf("=== REGISTERS ===\n");
    for(int i = 0; i < 4; i++) {
        printf("R%d: 0x%02X (%3d)  ", i, cpu->R[i], cpu->R[i]);
        if(i == 1) printf("\n");
    }
    printf("\nPC: 0x%04X SP: 0x%04X FLAGS: Z=%d C=%d\n", cpu->PC, cpu->SP, (cpu->FLAGS & FLAG_ZERO) ? 1 : 0, (cpu->FLAGS & FLAG_CARRY) ? 1 : 0);
}

void debugger_print_memory(struct CPU* cpu, uint16_t address, int lines) {
    printf("=== MEMORY at 0x%04X ===\n", address);
    for(int i = 0; i < lines; i++) {
        printf("0x%04X: ", address + i * 8);
        for(int j = 0; j < 8; j++) {
            uint16_t current_addr = address + i * 8 + j;
            if(current_addr < MEMORY_SIZE) {
                printf("%02X ", cpu->memory[current_addr]);
            }
        }
        printf("\n");
    }
}

void debugger_trace_step(struct CPU* cpu, uint8_t instruction) {
    cpu->debugger.last_pc = cpu->PC;
    if(cpu->debugger.trace_mode == 1) {
        printf("[TRACE] PC=0x%04X, Instruction=0x%02X \n", cpu->debugger.last_pc, instruction);
        debugger_print_registers(cpu);
        printf("\n");
    }
}