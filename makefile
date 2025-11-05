CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
SOURCES = main.c core/cpu.c assembler/assembler.c debugger/debugger.c peripherals/gpu.c peripherals/timer.c
TARGET = emulator

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

run:
	./emulator examples/demo.asm

debug:
	./emulator examples/demo.asm --debug

trace:
	./emulator examples/demo.asm --trace

.PHONY: clean run debug trace