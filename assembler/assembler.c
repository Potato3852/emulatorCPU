#include "../assembler/assembler.h"
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
            printf("DEBUG PASS1: Found label '%s' at address 0x%04X, previous instruction: '%s'\n", label_name, address, line);

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
            else if (strcmp(line, "ST") == 0) address += 4;
            else if (strcmp(line, "LD") == 0) address += 4;
            else if (strcmp(line, "PUSH") == 0) address += 2;
            else if (strcmp(line, "POP") == 0) address += 2;
            else if (strcmp(line, "INT") == 0) address += 2;
            else if (strcmp(line, "IRET") == 0) address += 1;
            else if (strcmp(line, "EI") == 0) address += 1; 
            else if (strcmp(line, "DI") == 0) address += 1; 
            else if (strcmp(line, "DEC") == 0) address += 2;
            //...
            else if (strcmp(line, "GPU_CLEAR") == 0) address += 1;
            else if (strcmp(line, "GPU_DRAW") == 0) address += 4;
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

            while(*pos == ' ') pos++;

            int reg_index;
            if(sscanf(pos, "R%d", &reg_index) != 1) {
                printf("ERROR: Can`t read reg after LDI!\n");
                return;
            }
            memory_write(cpu, address++, reg_index);

            while(*pos != ',' && *pos != '\n' && *pos != '\0') pos++;
            if(*pos != ',') {
                printf("ERROR: Can`t comma after reg\n");
                return;
            }
            pos++;

            while(*pos == ' ') pos++;

            char value_str[32];
            int i = 0;
            while(*pos != ' ' && *pos != '\n' && *pos != '\0' && *pos != ',') {
                if(i < 31) {
                    value_str[i++] = *pos;
                }
                pos++;
            }
            value_str[i] = '\0';

            int value;

            if(strstr(value_str, "&")) {
                printf("DEBUG: Found & expression\n");

                char* parts[2];
                parts[0] = strtok(value_str, "&");
                parts[1] = strtok(NULL, "&");
        
                if (!parts[0] || !parts[1]) {
                    printf("ERROR: Invalid expression format: %s\n", value_str);
                    return;
                }

                while(*parts[0] == ' ') parts[0]++;
                while(*parts[1] == ' ' && *parts[1] != '\0') parts[1]++;

                int label_addr = find_label_address(state, parts[0]);
                if (label_addr == -1) {
                    printf("ERROR: Label '%s' not found in expression!\n", parts[0]);
                    return;
                }

                int mask;
                if (parts[1][0] == '0' && (parts[1][1] == 'x' || parts[1][1] == 'X')) {
                    mask = (int)strtol(parts[1] + 2, NULL, 16);
                } else {
                    mask = atoi(parts[1]);
                }
                
                value = label_addr & mask;
                printf("DEBUG: LDI %s & %s -> 0x%04X & 0x%04X = 0x%02X\n", 
                    parts[0], parts[1], label_addr, mask, value);
            
            } else if(strstr(value_str, ">>")) {
                printf("DEBUG: Found >> expression\n"); 

                char* parts[2];
                parts[0] = strtok(value_str, ">>");
                parts[1] = strtok(NULL, ">>");
                
                if (!parts[0] || !parts[1]) {
                    printf("ERROR: Invalid expression format: %s\n", value_str);
                    return;
                }

                while(*parts[0] == ' ') parts[0];
                while(*parts[1] == ' ' && *parts[1] != '\0') parts[1]++;

                int label_addr = find_label_address(state, parts[0]);
                if (label_addr == -1) {
                    printf("ERROR: Label '%s' not found in expression!\n", parts[0]);
                    return;
                }

                int shift = atoi(parts[1]);
                value = label_addr >> shift;
                
                printf("DEBUG: LDI %s >> %d -> 0x%04X >> %d = 0x%02X\n", 
                    parts[0], shift, label_addr, shift, value);
            } else if(strstr(value_str, "<<")) {
                char* parts[2];
                parts[0] = strtok(value_str, "<<");
                parts[1] = strtok(NULL, "<<");
                
                if (!parts[0] || !parts[1]) {
                    printf("ERROR: Invalid expression format: %s\n", value_str);
                    return;
                }
                
                while(*parts[0] == ' ') parts[0]++;
                while(*parts[1] == ' ' && *parts[1] != '\0') parts[1]++;
                
                int label_addr = find_label_address(state, parts[0]);
                if (label_addr == -1) {
                    printf("ERROR: Label '%s' not found in expression!\n", parts[0]);
                    return;
                }
                
                int shift = atoi(parts[1]);
                value = label_addr << shift;
                
                printf("DEBUG: LDI %s << %d -> 0x%04X << %d = 0x%02X\n", 
                    parts[0], shift, label_addr, shift, value);
                
            } else {
                int label_addr = find_label_address(state, value_str);
                if (label_addr != -1) {
                    value = label_addr;
                    printf("DEBUG: LDI label %s -> 0x%04X\n", value_str, value);
                } else if(value_str[0] == '0' && (value_str[1] == 'x' || value_str[1] == 'X')) {
                    value = (int)strtol(value_str + 2, NULL, 16);
                    // DEBUG::
                    printf("DEBUG: LDI hex value %s -> %d (0x%02X)\n", value_str, value, value);
                } else {
                    value = atoi(value_str);
                    // DEBUG::
                    printf("DEBUG: LDI decimal value %s -> %d (0x%02X)\n", value_str, value, value);
                }
            }
            memory_write(cpu, address++, value);

        } else if(strcmp(line, "HLT") == 0) {
            memory_write(cpu, address++, OP_HLT);

        } else if(strcmp(line, "ADD") == 0) {
            memory_write(cpu, address++, OP_ADD);

            while (*pos == ' ' || *pos == '\t') pos++;
            
            if(*pos != 'R') {
                printf("ERROR: Can't read reg after ADD!\n");
                return;
            }
            pos++;
            int reg_a = *pos - '0';
            memory_write(cpu, address++, reg_a);
            pos++;

            while(*pos != ',' && *pos != ';' && *pos != '\n' && *pos != '\0') pos++;
            if(*pos != ',') {
                printf("ERROR: Expected comma after first register, got '%c'\n", *pos);
                return;
            }
            pos++;
            
            while(*pos == ' ' || *pos == '\t') pos++;

            if(*pos == ';' || *pos == '\n' || *pos == '\0') {
                printf("ERROR: Missing second register after comma in ADD\n");
                return;
            }

            if(*pos != 'R') {
                printf("ERROR: Can't read second reg! found '%c' (0x%02X)\n", *pos, *pos);
                return;
            }
            pos++;
            int reg_b = *pos - '0';
            memory_write(cpu, address++, reg_b);

            // DEBUG::
            printf("DEBUG: ADD R%d, R%d\n", reg_a, reg_b);

        } else if(strcmp(line, "SUB") == 0) {
            memory_write(cpu, address++, OP_SUB);

            while(*pos == ' ' || *pos == '\t') pos++;
            
            if(*pos != 'R') {
                printf("ERROR: Can't read reg after SUB!\n");
                return;
            }
            pos++;
            int reg_a = *pos - '0';
            memory_write(cpu, address++, reg_a);
            pos++;
            
            while(*pos != ',' && *pos != ';' && *pos != '\n' && *pos != '\0') pos++;
            if(*pos != ',') {
                printf("ERROR: Expected comma after first register, got '%c'\n", *pos);
                return;
            }
            pos++;

            while(*pos == ' ' || *pos == '\t') pos++;

            if(*pos == ';' || *pos == '\n' || *pos == '\0') {
                printf("ERROR: Missing second register in SUB\n");
                return;
            }
        
            if(*pos != 'R') {
                printf("ERROR: Can't read second reg!\n");
                return;
            }
            pos++;
            int reg_b = *pos - '0';
            memory_write(cpu, address++, reg_b);

            // DEBUG::
            // printf("DEBUG: SUB R%d, R%d\n", reg_a, reg_b);

        } else if(strcmp(line, "JMP") == 0) {
            memory_write(cpu, address++, OP_JMP);
    
            while (*pos == ' ') pos++;
            
            if(*pos == '0' && (*(pos+1) == 'x' || *(pos+1) == 'X')) {

                pos += 2;
                uint16_t jmp_addr = 0;
                while((*pos >= '0' && *pos <= '9') || 
                    (*pos >= 'a' && *pos <= 'f') || 
                    (*pos >= 'A' && *pos <= 'F')) {
                    uint8_t digit;
                    if(*pos >= '0' && *pos <= '9') digit = *pos - '0';
                    else if(*pos >= 'a' && *pos <= 'f') digit = *pos - 'a' + 10;
                    else digit = *pos - 'A' + 10;
                    jmp_addr = (jmp_addr << 4) | digit;
                    pos++;
                }
                memory_write(cpu, address++, jmp_addr & 0xFF);
                memory_write(cpu, address++, (jmp_addr >> 8) & 0xFF);
                printf("JMP 0x%04X\n", jmp_addr);
                
            } else {
                char label_name[32];
                int i = 0;
                while (*pos != ' ' && *pos != '\n' && *pos != '\0' && *pos != ',') {
                    label_name[i++] = *pos++;
                }
                label_name[i] = '\0';
                
                int label_address = find_label_address(state, label_name);
                if (label_address == -1) {
                    printf("ERROR: Label '%s' not found!\n", label_name);
                    return;
                }
                
                memory_write(cpu, address++, label_address & 0xFF);
                memory_write(cpu, address++, (label_address >> 8) & 0xFF);
                printf("JMP %s -> 0x%04X\n", label_name, label_address);
            }

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
                printf("ERROR: Label '%s' not found!\n", label_name);
                return;
            }
            
            memory_write(cpu, address++, label_address & 0xFF);
            memory_write(cpu, address++, (label_address >> 8) & 0xFF);
            
            printf("JZ %s -> 0x%04X\n", label_name, label_address);

        } else if(strcmp(line, "MOV") == 0) {
            memory_write(cpu, address++, OP_MOV);

            while(*pos == ' ' || *pos == '\t') pos++;
            
            if(*pos != 'R') {
                printf("ERROR: Can't read reg after MOV!\n");
                return;
            }
            pos++;
            int dest_reg = *pos - '0';
            memory_write(cpu, address++, dest_reg);
            pos++;
            
            while(*pos != ',' && *pos != ';' && *pos != '\n' && *pos != '\0') pos++;
            if(*pos != ',') {
                printf("ERROR: Expected comma after first register, got '%c'\n", *pos);
                return;
            }
            pos++;
            
            while(*pos == ' ' || *pos == '\t') pos++;

            if(*pos == ';' || *pos == '\n' || *pos == '\0') {
                printf("ERROR: Missing second register in MOV\n");
                return;
            }
        
            if(*pos != 'R') {
                printf("ERROR: Can't read second reg!\n");
                return;
            }
            pos++;
            int src_reg = *pos - '0';
            memory_write(cpu, address++, src_reg);

            // DEBUG::
            // printf("DEBUG: MOV R%d, R%d\n", dest_reg, src_reg);

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

            while(*pos == ' ' || *pos == '\t') pos++;
            
            if(*pos != 'R') {
                printf("ERROR: Can't read reg after ADD!\n");
                return;
            }
            pos++;
            int reg_a = *pos - '0';
            memory_write(cpu, address++, reg_a);
            pos++;
            
            while(*pos != ',' && *pos != ';' && *pos != '\n' && *pos != '\0') pos++;
            if(*pos != ',') {
                printf("ERROR: Expected comma after first register, got '%c'\n", *pos);
                return;
            }
            pos++;
            
            while(*pos == ' ' || *pos == '\t') pos++;

            if(*pos == ';' || *pos == '\n' || *pos == '\0') {
                printf("ERROR: Missing second register in CMP\n");
                return;
            }

            if(*pos != 'R') {
                printf("ERROR: Can't read second reg!\n");
                return;
            }
            pos++;
            int reg_b = *pos - '0';
            memory_write(cpu, address++, reg_b);

            // DEBUG::
            // printf("DEBUG: CMP R%d, R%d\n", reg_a, reg_b);

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
                printf("ERROR: Label '%s' not found!\n", label_name);
                return;
            }
            
            memory_write(cpu, address++, label_address & 0xFF);
            memory_write(cpu, address++, (label_address >> 8) & 0xFF);
            
            // printf("JNZ %s -> 0x%04X\n", label_name, label_address);

        } else if(strcmp(line, "ST") == 0) {
            memory_write(cpu, address++, OP_ST);

            char rest_of_line[256];
            int i = 0;
            while(*pos != '\n' && *pos != '\0' && i < 255) {
                rest_of_line[i++] = *pos++;
            }
            rest_of_line[i] = '\0';

            char* line_pos = rest_of_line;
            
            while(*line_pos == ' ') line_pos++;

            if(*line_pos != 'R') {
                printf("ERROR: Can't read reg after ST\n");
                return;
            }
            line_pos++;
            uint8_t reg = *line_pos - '0';
            memory_write(cpu, address++, reg);

            while(*line_pos != ',' && *line_pos != '\0') line_pos++;
            if(*line_pos != ',') {
                printf("ERROR: Expected comma after register\n");
                return;
            }
            line_pos++;

            while(*line_pos == ' ') line_pos++;

            if(*line_pos == '[') {
                line_pos++;
                while(*line_pos == ' ') line_pos++;
            }
            
            if(*line_pos == 'R') {

                line_pos++;
                uint8_t addr_reg = *line_pos - '0';
                line_pos++;

                while(*line_pos == ' ') line_pos++;

                if(*line_pos == ']') {

                    memory_write(cpu, address++, ADDR_INDIRECT);
                    memory_write(cpu, address++, addr_reg);
                } else if(*line_pos == '+') {

                    line_pos++;
                    while(*line_pos == ' ') line_pos++;

                    uint16_t offset;
                    if(sscanf(line_pos, "%hu", &offset) == 1) {
                        memory_write(cpu, address++, ADDR_INDEXED);
                        memory_write(cpu, address++, addr_reg);
                        memory_write(cpu, address++, offset & 0xFF);
                        memory_write(cpu, address++, (offset >> 8) & 0xFF);
                    }
                }
            } else {
                uint16_t mem_addr = 0;

                if(*line_pos == '0' && (*(line_pos+1) == 'x' || *(line_pos+1) == 'X')) {
                    line_pos += 2;
                    while((*line_pos >= '0' && *line_pos <= '9') || 
                        (*line_pos >= 'a' && *line_pos <= 'f') || 
                        (*line_pos >= 'A' && *line_pos <= 'F')) {
                        uint8_t digit;
                        if(*line_pos >= '0' && *line_pos <= '9') digit = *line_pos - '0';
                        else if(*line_pos >= 'a' && *line_pos <= 'f') digit = *line_pos - 'a' + 10;
                        else digit = *line_pos - 'A' + 10;
                        mem_addr = (mem_addr << 4) | digit;
                        line_pos++;
                    }
                } else {
                    while(*line_pos >= '0' && *line_pos <= '9') {
                        mem_addr = mem_addr * 10 + (*line_pos - '0');
                        line_pos++;
                    }
                }

                memory_write(cpu, address++, ADDR_DIRECT);
                memory_write(cpu, address++, mem_addr & 0xFF);
                memory_write(cpu, address++, (mem_addr >> 8) & 0xFF);
            }

            while(*pos != '\n' && *pos != '\0') pos++;
            if(*pos == '\n') pos++; 

        } else if(strcmp(line, "LD") == 0) {
            memory_write(cpu, address++, OP_LD);

            char rest_of_line[256];
            int i = 0;
            while(*pos != '\n' && *pos != '\0' && i < 255) {
                rest_of_line[i++] = *pos++;
            }
            rest_of_line[i] = '\0';

            char* line_pos = rest_of_line;

            while(*line_pos == ' ') line_pos++;

            if(*line_pos != 'R') {
                printf("ERROR: Can't read reg after LD\n");
                return;
            }
            line_pos++;
            uint8_t reg = *line_pos - '0';
            memory_write(cpu, address++, reg);

            while(*line_pos != ',' && *line_pos != '\0') line_pos++;
            if(*line_pos != ',') {
                printf("ERROR: Can't read comma after register\n");
                return;
            }
            line_pos++;

            while(*line_pos == ' ') line_pos++;

            if(*line_pos == '[') {
                line_pos++;
                while(*line_pos == ' ') line_pos++;
            }
            
            if(*line_pos == 'R') {
                line_pos++;
                uint8_t addr_reg = *line_pos - '0';
                line_pos++;

                while(*line_pos == ' ') line_pos++;

                if(*line_pos == ']') {
                    memory_write(cpu, address++, ADDR_INDIRECT);
                    memory_write(cpu, address++, addr_reg);
                } else if(*line_pos == '+') {
                    line_pos++;
                    while(*line_pos == ' ') line_pos++;

                    uint16_t offset;
                    if(sscanf(line_pos, "%hu", &offset) == 1) {
                        memory_write(cpu, address++, ADDR_INDEXED);
                        memory_write(cpu, address++, addr_reg);
                        memory_write(cpu, address++, offset & 0xFF);
                        memory_write(cpu, address++, (offset >> 8) & 0xFF);
                    }
                }
            } else {
                uint16_t mem_addr = 0;

                if(*line_pos == '0' && (*(line_pos+1) == 'x' || *(line_pos+1) == 'X')) {
                    line_pos += 2;
                    while((*line_pos >= '0' && *line_pos <= '9') || 
                        (*line_pos >= 'a' && *line_pos <= 'f') || 
                        (*line_pos >= 'A' && *line_pos <= 'F')) {
                        uint8_t digit;
                        if(*line_pos >= '0' && *line_pos <= '9') digit = *line_pos - '0';
                        else if(*line_pos >= 'a' && *line_pos <= 'f') digit = *line_pos - 'a' + 10;
                        else digit = *line_pos - 'A' + 10;
                        mem_addr = (mem_addr << 4) | digit;
                        line_pos++;
                    }
                } else {
                    while(*line_pos >= '0' && *line_pos <= '9') {
                        mem_addr = mem_addr * 10 + (*line_pos - '0');
                        line_pos++;
                    }
                }

                memory_write(cpu, address++, ADDR_DIRECT);
                memory_write(cpu, address++, mem_addr & 0xFF);
                memory_write(cpu, address++, (mem_addr >> 8) & 0xFF);
            }

            while(*pos != '\n' && *pos != '\0') pos++;
            if(*pos == '\n') pos++;

        } else if(strcmp(line, "PUSH") == 0) {
            memory_write(cpu, address++, OP_PUSH);

            while(*pos == ' ') pos++;

            if(*pos != 'R') {
                printf("ERROR: Can't read reg after PUSH\n");
                return;
            }
            pos++;
            uint8_t reg = *pos - '0';
            memory_write(cpu, address++, reg);

        } else if(strcmp(line, "POP") == 0) {
            memory_write(cpu, address++, OP_POP);

            while(*pos == ' ') pos++;

            if(*pos != 'R') {
                printf("ERRROR: Can't read reg after POP\n");
                return;
            }
            pos++;
            uint8_t reg = *pos - '0';
            memory_write(cpu, address++, reg);

        } else if(strcmp(line, "INT") == 0) {
            memory_write(cpu, address++, OP_INT);

            while(*pos == ' ') pos++;

            int vector;
            if(sscanf(pos, "%d", &vector) != 1) {
                printf("ERROR: Can't read interrupt vector!\n");
                return;
            }
            memory_write(cpu, address++, vector);

        } else if(strcmp(line, "IRET") == 0) {
            memory_write(cpu, address++, OP_IRET);
        
        } else if(strcmp(line, "EI") == 0) {
            memory_write(cpu, address++, OP_EI);

        } else if(strcmp(line, "DI") == 0) {
            memory_write(cpu, address++, OP_DI);

        } else if(strcmp(line, "DEC") == 0) {
            memory_write(cpu, address++, OP_DEC);

            while(*pos == ' ') pos++;

            if(*pos != 'R') {
                printf("ERROR: Can't read reg after DEC!\n");
                return;
            }
            pos++;
            int reg = *pos - '0';
            
            memory_write(cpu, address++, reg);

        } else if(strcmp(line, "GPU_CLEAR") == 0) {
            memory_write(cpu, address++, OP_GPU_CLEAR);

        } else if(strcmp(line, "GPU_DRAW") == 0) {
            memory_write(cpu, address++, OP_GPU_DRAW);

            while(*pos == ' ') pos++;

            int x = atoi(pos);
            memory_write(cpu, address++, x);

            while(*pos != ',' && *pos != '\0') pos++;
            if(*pos == ',') pos++;
            while(*pos == ' ') pos++;

            int y = atoi(pos);
            memory_write(cpu, address++, y);

            while(*pos != ',' && *pos != '\0') pos++;
            if(*pos == ',') pos++;
            while(*pos == ' ') pos++;

            int color = atoi(pos);
            memory_write(cpu, address++, color);

            // DEBUG::
            // printf("DEBUG: GPU_DRAW %d, %d, %d\n", x, y, color);
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

    printf("=== Memory dump around main (0x002A) ===\n");
    for(int i = 0x0025; i <= 0x0030; i++) {
        printf("0x%04X: 0x%02X\n", i, cpu->memory[i]);
    }
}