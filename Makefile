CC = gcc
CFLAG = -std=c99 -gWall
OBJS = main.o dump_asm.o elf32.o 

all: riscv32-disassembler

riscv32-disassembler: $(OBJS)
	gcc -o riscv32-disassembler $^

%.o: %.c
	gcc -c -o $@ $<

clean:
	rm -f riscv32-disassembler $(OBJS)
