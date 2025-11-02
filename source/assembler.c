#include "assembler.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int is_label(const char* line) {
    int len = strlen(line);
    return (len > 0 && line[len-1] == ':');
}

void get_label_name(const char* label_with_colon, char* output) {
    int i = 0;
    while(label_with_colon[i] != ':' && label_with_colon[i] != '\0') {
        output[i] = label_with_colon[i];
        i++;
    }
    output[i] = '\0';
}

int find_label_address(struct AssemblerState* state, const char* name) {
    for(int i = 0; i < state->label_count; i++) {
        if(strcmp(state->labels[i].name, name) == 0) {
            return state->labels[i].address;
        }
    }
    return -1;
}

void assemble_pass1(struct AssemblerState* state, const char* code, uint16_t start_address) {
    char line[256];
    const char* pos = code;
    uint16_t address = start_address;

    state->label_count = 0;

    while(*pos) {
        while(*pos == ' ' || *pos == '\t') pos++;
        if(*pos == '\n' || *pos == '\0') {
            pos++;
            continue;
        }

        int i = 0;
        while(*pos != ' ' && *pos != '\n' && *pos != '\0' && *pos != ',') {
            line[i++] = *pos++;
        }
        line[i] = '\0';

        if(is_label(line)) {
            char label_name[32];
            get_label_name(line, label_name);

            if(state->label_count < MAX_LABELS) {
                strcpy(state->labels[state->label_count].name, label_name);
                state->labels[state->label_count].address = address;
                state->label_count++;
                printf("Find new label: %s -> 0x%04X\n", label_name, address);
            }
        } else {
            if (strcmp(line, "LDI") == 0) address += 3;
            else if (strcmp(line, "ADD") == 0) address += 3;
            else if (strcmp(line, "SUB") == 0) address += 3;
            else if (strcmp(line, "MOV") == 0) address += 3;
            else if (strcmp(line, "CMP") == 0) address += 3;
            else if (strcmp(line, "JMP") == 0) address += 3;
            else if (strcmp(line, "JZ") == 0) address += 3;
            else if (strcmp(line, "JNZ") == 0) address += 3;
            else if (strcmp(line, "INC") == 0) address += 2;
            else if (strcmp(line, "HLT") == 0) address += 1;
        }

        while(*pos != '\n' && *pos != '\0') pos++;
        if(*pos == '\n') pos++;
    }
}

void assemble_pass2(struct AssemblerState* state, struct CPU* cpu, const char* code, uint16_t start_address) {
    char line[256];
    const char* pos = code;
    uint16_t address = start_address;

    while(*pos) {
        while(*pos == ' ' || *pos == '\t') pos++;
        
        if(*pos == '\n' || *pos == '\0') {
            pos++;
            continue;
        }

        int i = 0;
        while(*pos != ' ' && *pos != '\n' && *pos != '\0' && *pos != ',') {
            line[i++] = *pos++;
        }
        line[i] = '\0';

        if(is_label(line)) {
        } else if(strcmp(line, "LDI") == 0) {
            memory_write(cpu, address++, OP_LDI);

            while (*pos == ' ') pos++;
            
            int reg_index;
            if(sscanf(pos, "R%d", &reg_index) != 1) {
                printf("ERROR: Can't read register after LDI!\n");
                return;
            }
            memory_write(cpu, address++, reg_index);
            
            while(*pos != ',' && *pos != '\n' && *pos != '\0') pos++;
            if(*pos != ',') {
                printf("ERROR: Expected comma after register\n");
                return;
            }
            pos++;
            
            while(*pos == ' ') pos++;
            
            int value;
            if(sscanf(pos, "%d", &value) != 1) {
                printf("ERROR: Can't read value!\n");
                return;
            }
            memory_write(cpu, address++, value);
        } else if(strcmp(line, "HLT") == 0) {
            memory_write(cpu, address++, OP_HLT);
        } else if(strcmp(line, "ADD") == 0) {
            memory_write(cpu, address++, OP_ADD);

            while (*pos == ' ') pos++;
            
            int reg_a;
            if(sscanf(pos, "R%d", &reg_a) != 1) {
                printf("ERROR: Can't read reg_a after ADD!\n");
                return;
            }
            memory_write(cpu, address++, reg_a);
            
            while(*pos != ',' && *pos != '\n' && *pos != '\0') pos++;
            if(*pos != ',') {
                printf("ERROR: Expected comma after first register\n");
                return;
            }
            pos++;
            
            while(*pos == ' ') pos++;
            
            int reg_b;
            if(sscanf(pos, "R%d", &reg_b) != 1) {
                printf("ERROR: Can't read reg_b!\n");
                return;
            }
            memory_write(cpu, address++, reg_b);
        } else if(strcmp(line, "SUB") == 0) {
            memory_write(cpu, address++, OP_SUB);

            while (*pos == ' ') pos++;
            
            int reg_a;
            if(sscanf(pos, "R%d", &reg_a) != 1) {
                printf("ERROR: Can't read reg_a after SUB!\n");
                return;
            }
            memory_write(cpu, address++, reg_a);
            
            while(*pos != ',' && *pos != '\n' && *pos != '\0') pos++;
            if(*pos != ',') {
                printf("ERROR: Expected comma after first register\n");
                return;
            }
            pos++;
            
            while(*pos == ' ') pos++;
            
            int reg_b;
            if(sscanf(pos, "R%d", &reg_b) != 1) {
                printf("ERROR: Can't read reg_b!\n");
                return;
            }
            memory_write(cpu, address++, reg_b);
        } else if(strcmp(line, "JMP") == 0) {
            memory_write(cpu, address++, OP_JMP);
        
            while (*pos == ' ') pos++;
            
            char label_name[32];
            i = 0;
            while (*pos != ' ' && *pos != '\n' && *pos != '\0' && *pos != ',') {
                label_name[i++] = *pos++;
            }
            label_name[i] = '\0';
            
            int label_address = find_label_address(state, label_name);
            if (label_address == -1) {
                printf("ОШИБКА: Метка '%s' не найдена!\n", label_name);
                return;
            }
            
            memory_write(cpu, address++, label_address & 0xFF);
            memory_write(cpu, address++, (label_address >> 8) & 0xFF);
            
            printf("JMP %s -> 0x%04X\n", label_name, label_address);
        } else if(strcmp(line, "JZ") == 0) {
            memory_write(cpu, address++, OP_JZ);
        
            while (*pos == ' ') pos++;
            
            char label_name[32];
            i = 0;
            while (*pos != ' ' && *pos != '\n' && *pos != '\0' && *pos != ',') {
                label_name[i++] = *pos++;
            }
            label_name[i] = '\0';
            
            int label_address = find_label_address(state, label_name);
            if (label_address == -1) {
                printf("ОШИБКА: Метка '%s' не найдена!\n", label_name);
                return;
            }
            
            memory_write(cpu, address++, label_address & 0xFF);
            memory_write(cpu, address++, (label_address >> 8) & 0xFF);
            
            printf("JZ %s -> 0x%04X\n", label_name, label_address);
        } else if(strcmp(line, "MOV") == 0) {
            memory_write(cpu, address++, OP_MOV);

            while (*pos == ' ') pos++;
            
            int dest_reg;
            if(sscanf(pos, "R%d", &dest_reg) != 1) {
                printf("ERROR: Can't read dest_reg after MOV!\n");
                return;
            }
            memory_write(cpu, address++, dest_reg);
            
            while(*pos != ',' && *pos != '\n' && *pos != '\0') pos++;
            if(*pos != ',') {
                printf("ERROR: Expected comma after first register\n");
                return;
            }
            pos++;
            
            while(*pos == ' ') pos++;
            
            int src_reg;
            if(sscanf(pos, "R%d", &src_reg) != 1) {
                printf("ERROR: Can't read src_reg!\n");
                return;
            }
            memory_write(cpu, address++, src_reg);
        } else if(strcmp(line, "INC") == 0) {
            memory_write(cpu, address++, OP_INC);

            while (*pos == ' ') pos++;
            
            int reg;
            if(sscanf(pos, "R%d", &reg) != 1) {
                printf("ERROR: Can't read reg after INC!\n");
                return;
            }
            memory_write(cpu, address++, reg);
        } else if(strcmp(line, "CMP") == 0) {
            memory_write(cpu, address++, OP_CMP);

            while (*pos == ' ') pos++;
            
            int reg_a;
            if(sscanf(pos, "R%d", &reg_a) != 1) {
                printf("ERROR: Can't read reg_a after CMP!\n");
                return;
            }
            memory_write(cpu, address++, reg_a);
            
            while(*pos != ',' && *pos != '\n' && *pos != '\0') pos++;
            if(*pos != ',') {
                printf("ERROR: Expected comma after first register\n");
                return;
            }
            pos++;
            
            while(*pos == ' ') pos++;
            
            int reg_b;
            if(sscanf(pos, "R%d", &reg_b) != 1) {
                printf("ERROR: Can't read reg_b!\n");
                return;
            }
            memory_write(cpu, address++, reg_b);
        } else if(strcmp(line, "JNZ") == 0) {
            memory_write(cpu, address++, OP_JNZ);
        
            while (*pos == ' ') pos++;
            
            char label_name[32];
            i = 0;
            while (*pos != ' ' && *pos != '\n' && *pos != '\0' && *pos != ',') {
                label_name[i++] = *pos++;
            }
            label_name[i] = '\0';
            
            int label_address = find_label_address(state, label_name);
            if (label_address == -1) {
                printf("ОШИБКА: Метка '%s' не найдена!\n", label_name);
                return;
            }
            
            memory_write(cpu, address++, label_address & 0xFF);
            memory_write(cpu, address++, (label_address >> 8) & 0xFF);
            
            printf("JNZ %s -> 0x%04X\n", label_name, label_address);
        }

        while(*pos != '\n' && *pos != '\0') pos++;
        if(*pos == '\n') pos++;
    }
}

void assemble(struct CPU* cpu, const char* code, uint16_t start_address) {
    struct AssemblerState state;

    printf("=== Pass 1: Finding labels ===\n");
    assemble_pass1(&state, code, start_address);
    
    printf("=== Pass 2: Generation code ===\n");
    assemble_pass2(&state, cpu, code, start_address);
    
    printf("Assembling complete. Found %d labels\n", state.label_count);
}