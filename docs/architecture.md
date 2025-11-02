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