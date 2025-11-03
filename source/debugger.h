#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "cpu.h"

void debugger_init(struct Debugger* dbg);
void debugger_add_breakpoint(struct Debugger* dbg, uint16_t address);
void debugger_remove_breakpoint(struct Debugger* dbg, uint16_t address);
void debugger_check_breakpoint(struct CPU* cpu);
void debugger_interactive(struct CPU* cpu);
void debugger_print_registers(struct CPU* cpu);
void debugger_print_memory(struct CPU* cpu, uint16_t address, int lines);
void debugger_trace_step(struct CPU* cpu, uint8_t instruction);

#endif