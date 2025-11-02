#include "cpu.h"
#include<stdio.h>

void cpu_init(struct CPU* cpu) {
    for(int i = 0; i < 4; i++) {
        cpu->R[i] = 0;
    }

    cpu->PC = 0;
    cpu->SP = 0x200;
    cpu->FLAGS = 0;
    cpu->running = 1;

    for(int i = 0; i < MEMORY_SIZE; ++i) {
        cpu->memory[i] = 0;
    }
}

void cpu_print_state(struct CPU* cpu) {
    printf("=== CPU STATE ===\n");
    for(int i = 0; i < 4; ++i) {
        printf("R[%d]: 0x%02X (%d)\n", i, cpu->R[i], cpu->R[i]);
    }
    printf("PC: 0x%04X\n", cpu->PC);
    printf("SP: 0x%04X\n", cpu->SP);
    printf("FLAGS: Z=%d C=%d (0x%02X)\n", (cpu->FLAGS & FLAG_ZERO) ? 1 : 0, (cpu->FLAGS & FLAG_CARRY) ? 1 : 0, cpu->FLAGS);
    printf("RUNNING: %d\n", cpu->running);
}

void memory_write(struct CPU* cpu, uint16_t address, uint8_t value) {
    if(address < MEMORY_SIZE) {
        cpu->memory[address] = value;
    }
}

uint8_t memory_read(struct CPU* cpu, uint16_t address) {
    if(address < MEMORY_SIZE) {
        return cpu->memory[address];
    }
    return 0;
}

void cpu_step(struct CPU* cpu) {
    uint8_t instruction = memory_read(cpu, cpu->PC);

    //DEBUG:: printf("DEBUG: PC=0x%04X, instruction=0x%02X, R0=%d\n", cpu->PC, instruction, cpu->R[0]);

    cpu->PC++;

    switch(instruction) {
        case OP_HLT:
            cpu->running = 0;
            break;

        case OP_LDI: {
                uint8_t reg_index = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t value = memory_read(cpu, cpu->PC);
                cpu->PC++;

                if(reg_index < 4) {
                    cpu->R[reg_index] = value;
                }
                break;
            }

        case OP_ADD: {
                uint8_t dest_reg = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t src_reg = memory_read(cpu, cpu->PC);
                cpu->PC++;

                if(dest_reg < 4 && src_reg < 4) {
                    cpu->R[dest_reg] += cpu->R[src_reg];
                }

                update_flags(cpu, cpu->R[dest_reg]);

                break;
            }
        
        case OP_SUB: {
                uint8_t dest_reg = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t src_reg = memory_read(cpu, cpu->PC);
                cpu->PC++;

                if(dest_reg < 4 && src_reg < 4) {
                    cpu->R[dest_reg] -= cpu->R[src_reg];
                }

                update_flags(cpu, cpu->R[dest_reg]);

                break;
            }
        
        case OP_JMP: {
                uint8_t low_byte = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t high_byte = memory_read(cpu, cpu->PC);
                cpu->PC++;

                uint16_t jump_address = (high_byte << 8) | low_byte;
                cpu->PC = jump_address;

                break;
            }
        
        case OP_JZ: {
                uint8_t low_byte = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t high_byte = memory_read(cpu, cpu->PC);
                cpu->PC++;
                
                if (cpu->FLAGS & FLAG_ZERO) {
                    uint16_t jump_address = (high_byte << 8) | low_byte;
                    cpu->PC = jump_address;
                }
                break;
            }

        case OP_MOV: {
                uint8_t dest_reg = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t src_reg = memory_read(cpu, cpu->PC);
                cpu->PC++;

                if(dest_reg < 4 && src_reg < 4) {
                    cpu->R[dest_reg] = cpu->R[src_reg];
                }
                break;
            }
        
        case OP_INC: {
                uint8_t reg = memory_read(cpu, cpu->PC);
                cpu->PC++;

                if(reg < 4) {
                    cpu->R[reg]++;
                    update_flags(cpu, cpu->R[reg]);
                }
                break;
            }

        case OP_CMP: {
                uint8_t reg_a = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t reg_b = memory_read(cpu, cpu->PC);
                cpu->PC++;

                if(reg_a < 4 && reg_b < 4) {
                    uint8_t value_a = cpu->R[reg_a];
                    uint8_t value_b = cpu->R[reg_b];

                    uint8_t result = value_a - value_b;
                    update_flags(cpu, result);
                }
                break;
            }

        case OP_JNZ: {
                uint8_t low_byte = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t high_byte = memory_read(cpu, cpu->PC);
                cpu->PC++;

                //DEBUG:: uint16_t jmp_address = (high_byte << 8) | low_byte;
                //DEBUG:: printf("JNZ DEBUG: low=0x%02X, high=0x%02X, address=0x%04X, Z-flag=%d\n", low_byte, high_byte, jmp_address, (cpu->FLAGS & FLAG_ZERO) ? 1 : 0);

                if(!(cpu->FLAGS & FLAG_ZERO)) {
                    uint16_t jmp_address = (high_byte << 8) | low_byte;
                    cpu->PC = jmp_address;
                }
                break;
            }

        case OP_ST: {
                uint8_t reg = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t low_byte = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t high_byte = memory_read(cpu, cpu->PC);
                cpu->PC++;
                
                uint16_t mem_address = (high_byte << 8) | low_byte;
                
                if(reg < 4) {
                    memory_write(cpu, mem_address, cpu->R[reg]);
                    printf("DEBUG: ST R%d -> [0x%04X] (value: %d)\n", reg, mem_address, cpu->R[reg]);
                }
                break;
            }

        case OP_LD: {
                uint8_t reg = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t low_byte = memory_read(cpu, cpu->PC);
                cpu->PC++;
                uint8_t high_byte = memory_read(cpu, cpu->PC);
                cpu->PC++;
                
                uint16_t mem_address = (high_byte << 8) | low_byte;
                
                if(reg < 4) {
                    cpu->R[reg] = memory_read(cpu, mem_address);
                    printf("DEBUG: LD R%d <- [0x%04X] (value: %d)\n", reg, mem_address, cpu->R[reg]);
                }
                break;
            }  

        case OP_PUSH: {
                uint8_t reg = memory_read(cpu, cpu->PC);
                cpu->PC++;
                
                if(reg < 4) {
                    cpu->SP--;
                    memory_write(cpu, cpu->SP, cpu->R[reg]);
                    printf("DEBUG: PUSH R%d -> [SP=0x%04X]\n", reg, cpu->SP);
                }
                break;
            }

        case OP_POP: {
                uint8_t reg = memory_read(cpu, cpu->PC);
                cpu->PC++;
                
                if(reg < 4) {
                    cpu->R[reg] = memory_read(cpu, cpu->SP);
                    cpu->SP++;
                    printf("DEBUG: POP R%d <- [SP=0x%04X] (value: %d)\n", reg, cpu->SP, cpu->R[reg]);
                }
                break;
            }

        default:
            printf("Unknown instruction: 0x%02X at PC=0x%04X\n", instruction, cpu->PC);
            cpu->running = 0;
            break;
    }
}

void cpu_run(struct CPU* cpu) {
    printf("CPU running...\n");

    while(cpu->running) {
        cpu_step(cpu);
    }

    printf("CPU stopped.\n");
}

void update_flags(struct CPU* cpu, uint8_t result) {
    if(result == 0) {
        cpu->FLAGS |= FLAG_ZERO;
    } else {
        cpu->FLAGS &= ~FLAG_ZERO;
    }
}