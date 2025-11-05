# Simple CPU Emulator

A minimal 8-bit CPU emulator with integrated assembler written in C.

## Features

- **8-bit CPU** with 4 registers (R0-R3)
- **64KB memory** 
- **Built-in assembler** with label support
- **11 instructions**: HLT, LDI, ADD, SUB, MOV, INC, CMP, JMP, JZ, JNZ
- **Conditional jumps** based on flags
- **Step-by-step execution**

## Build & Run

```bash
gcc main.c assembler.c cpu.c -o emulator
./emulator
```

## Debugger Features

**Interactive Debugging:**
- `s` - step through instructions one by one  
- `c` - continue execution
- `b <addr>` - set breakpoint at address
- `bl` - list all breakpoints
- `rb <addr>` - remove breakpoint
- `m <addr>` - examine memory
- `r` - show registers
- `t` - toggle trace mode

**Usage:**
```bash
./emulator program.asm --debug    # Interactive debug mode
./emulator program.asm --trace    # Auto-trace execution
```

## Interrupt System
- **Software Interrupts**: `INT vector` - call interrupt handler
- **Interrupt Return**: `IRET` - return from interrupt  
- **Interrupt Control**: `EI`/`DI` - enable/disable interrupts
- **Memory-mapped Vector Table** - flexible interrupt handling

## Advanced Addressing System (**NEW**)
- **Direct addressing** - `[0x1234]`
- **Indirect addressing** - `[Rx]` 
- **Indexed addressing** - `[Rx + N]`
- **Complete ST/LD instruction support**

## Advanced GPU System ##
- 64x32 monochrome display with instant rendering
- **REAL-TIME FRAME DEBUGGER** - step through animation frames

## Precision Timer with Interrupts ##  
- MMIO-mapped timer at `0xFF00`
- Configurable intervals and interrupts

## 👥 Contributors

- Potato(me)
- The voices in my head
- Caffeine molecule
- My brother deeepseek

## 📜 License

This project is licensed under the "Look But Don't Steal My Bugs" license.