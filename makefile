emulator: source/main.c source/cpu.c source/assembler.c source/debugger.c
		gcc -Wall -o emulator source/main.c source/cpu.c source/assembler.c source/debugger.c

clean:
		rm -f emulator

run:
		./emulator ~/emulatorCPU/examples/memory_test.asm
