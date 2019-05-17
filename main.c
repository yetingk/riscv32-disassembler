#include "elf32.h"

extern int decode_binary(uint32_t);

int main(int argc, char **argv)
{
    const char *file_name = NULL;
    size_t code_size;
    uint32_t *code;

    if (argc < 2) {
        fatal("no file to disassemble\n");
        exit(1);
    }

    file_name = argv[1];

    elf32 *e = open_elf32(file_name);
    if (!e)
        return 1;

    code = find_section(e, ".text", &code_size);

    for (int i = 0; i < code_size / 4; ++i) {
        decode_binary(code[i]);
    }

    close_elf32(e);

    return 0;
}
