#include "elf32.h"

extern int decode_binary(void *);

int main(int argc, char **argv)
{
    const char *file_name = NULL;
    size_t code_size;
    void *code;
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
    for (int i = 0; i < code_size; i += sizeof(uint32_t)) {
        if (tmp && i == tmp->offset) {
            printf("\n%s:\n", tmp->name);
            tmp = tmp->next;
        }
        decode_binary(code + i);
    }

    // free allocated resource
    close_elf32(e);
    free_symlist(sym);
    return 0;
}
