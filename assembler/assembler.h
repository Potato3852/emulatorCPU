#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../core/cpu.h"

#define MAX_LABELS 50

struct Label {
    char name[32];
    uint16_t address;
};

struct AssemblerState {
    struct Label labels[MAX_LABELS];
    int label_count;
};

void assemble(struct CPU* cpu, const char* code, uint16_t start_address);

#endif