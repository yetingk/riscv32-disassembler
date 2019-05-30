#include "elf32.h"
#include "riscv32/instruction.h"

extern int dump_asm(int opcode, Instr *instr, uint32_t addr);

int main(int argc, char **argv)
{
    const char *file_name = NULL;
    size_t code_size;
    int opcode, *code;
    Instr instr;
    SymList *tmp, *sym;

    if (argc < 2) {
        fatal("no file to disassemble\n");
        exit(1);
    }

    file_name = argv[1];

    elf32 *e = open_elf32(file_name);
    if (!e)
        return 1;

    // find function name and address.
    tmp = sym = func_list(e);

    // find text section position
    code = find_section(e, ".text", &code_size);

    // dump asm
    for (int i = 0; i < code_size / sizeof(int); i++) {
        if (tmp && i == tmp->offset) {
            printf("\n%s:\n", tmp->name);
            tmp = tmp->next;
        }
        opcode = decode_instr(code[i], &instr);
        dump_asm(opcode, &instr, i << 2);
    }

    // free allocated resource
    close_elf32(e);
    free_symlist(sym);
    return 0;
}
