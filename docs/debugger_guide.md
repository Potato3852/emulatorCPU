# Debugger User Guide

## Quick Start
1. Compile with debug support
2. Run: `./emulator program.asm --debug`
3. Use commands to control execution

## Command Reference
| Command | Description |
|---------|-------------|
| `s` | Execute single instruction |
| `c` | Continue until breakpoint |
| `b 0x1234` | Set breakpoint |
| `m 0x1000` | Show memory at address |
| `r` | Show all registers |
| `t` | Toggle instruction tracing |
| `g` | Open GPU debugger |
| `help` | Show all commands |

## Debugging Techniques
- Set breakpoints before loops to inspect iterations
- Use memory watch to track variable changes  
- Trace mode shows execution flow
- Register inspection reveals calculation results

### GPU Debugging Features
- **Frame Stepping**: See each frame render in real-time
- **Cycle Counting**: Track performance between frames  
- **Register Inspection**: See CPU state between renders
- **Timer Integration**: Monitor timer interrupts affecting rendering