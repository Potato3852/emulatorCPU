# CPU Architecture

## Overview
8-bit educational CPU emulator with simplified von Neumann architecture.

## Registers

### General Purpose Registers (8-bit)
- **R0** - Primary accumulator, function return values
- **R1** - General purpose, function arguments
- **R2** - General purpose, function arguments  
- **R3** - General purpose, temporary storage

### Special Registers
- **PC** (16-bit) - Program Counter - points to next instruction
- **FLAGS** (8-bit) - Status flags register

## Memory
- **Size**: 64KB (65,536 bytes)
- **Organization**: Unified address space for code and data
- **Addressing**: 16-bit addresses, little-endian byte order

## Flags Register
Bit layout of FLAGS register:
7 6 5 4 3 2 1 0
| | | | | | | └ Zero Flag (Z)
| | | | | | └── Carry Flag (C)
└─┴─┴─┴─┴─┴──── Reserved (always 0)

### Flags Description:
- **Zero (Z)**: Set when operation result equals zero
- **Carry (C)**: Set when operation produces carry (reserved for future)

## Instruction Format
Instructions have variable length:
- 1 byte: `HLT`
- 2 bytes: `INC Rd`
- 3 bytes: `LDI Rd, value`, `ADD Rd, Rs`, `JMP address`

## Data Types
- **Integers**: 8-bit unsigned (0-255)
- **Addresses**: 16-bit unsigned (0-65535)

## Memory Access
- Direct addressing with ST/LD instructions
- Stack operations with PUSH/POP  
- 16-bit memory addresses

## Stack
- Grows downward (from higher to lower addresses)
- Stack Pointer (SP) starts at 0x0200
- LIFO (Last-In-First-Out) structure

## Debugging Support

The emulator includes a powerful debugger with:
- **Hardware breakpoints** - stop execution at specific addresses
- **Register inspection** - view all CPU state in real-time  
- **Memory examination** - peek at any memory location
- **Execution tracing** - log every instruction executed
- **Interactive control** - step through code manually

Debugger integration is seamless - simply compile with `--debug` flag.

### Display System
- **Resolution**: 64x32 pixels (1-bit monochrome)
- **Memory**: 256-byte VRAM at calculated addresses
- **Rendering**: Automatic console output every 100 cycles
- **Performance**: Dirty flag optimization - redraw only when changed

### GPU Instructions
- `OP_GPU_CLEAR (0x20)` - Clear entire screen
- `OP_GPU_DRAW (0x21)` - Draw pixel at (x, y) with color