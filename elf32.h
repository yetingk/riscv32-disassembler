#ifndef INCLUDE_ELF32_H
#define INCLUDE_ELF32_H

#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    void *file;
    ssize_t size;
    Elf32_Shdr *shdr;
    uint16_t shdr_num;
    const char *shdr_name;
} elf32;

elf32 *open_elf32(const char *file_name);
void close_elf32(elf32 *e);
void *find_section(elf32 *e, const char *section_name, ssize_t *size);

#define fatal(...) fprintf(stderr, __VA_ARGS__)

#endif /* INCLUDE_ELF32_H */
