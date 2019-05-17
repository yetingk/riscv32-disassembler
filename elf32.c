#include "elf32.h"

static inline ssize_t file_size(const char *file_name)
{
    struct stat st;
    ssize_t res = stat(file_name, &st);
    if (res < 0) {
        fatal("stat: %s: %s\n", file_name, strerror(errno));
        return -1;
    }
    return st.st_size;
}

static inline void set_section_data(elf32 *e)
{
    void *f = e->file;
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *) f;
    Elf32_Shdr *shdr = (Elf32_Shdr *) (e->file + ehdr->e_shoff);

    e->shdr = shdr;
    e->shdr_num = ehdr->e_shnum;
    e->shdr_name = (char *) (f + shdr[ehdr->e_shstrndx].sh_offset);
}


elf32 *open_elf32(const char *file_name)
{
    int fd = -1;
    struct stat st;
    elf32 *e = NULL;

    e = malloc(sizeof(elf32));
    if (!e) {
        fatal("malloc: %s\n", strerror(errno));
        return NULL;
    }

    e->size = file_size(file_name);
    if (e->size <= 0) {
        fatal("file_size: can't get %s size\n", file_name);
        goto file_fail;
    }

    fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        fatal("open: %s: %s\n", file_name, strerror(errno));
        goto file_fail;
    }

    e->file = mmap(NULL, e->size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (e->file == (void *) -1) {
        fatal("mmap: %s\n", strerror(errno));
        goto mmap_fail;
    }

    close(fd);
    set_section_data(e);
    return e;

mmap_fail:
    close(fd);
file_fail:
    free(e);
    return NULL;
}

void close_elf32(elf32 *e)
{
    munmap(e->file, e->size);
    free(e);
}


void *find_section(elf32 *e, const char *section_name, ssize_t *size)
{
    void *f;
    Elf32_Ehdr *ehdr;
    Elf32_Shdr *shdr;
    const char *tmp, *sh_name;
    uint16_t sh_num;

    f = e->file;
    shdr = e->shdr;
    sh_num = e->shdr_num;
    sh_name = e->shdr_name;

    for (int i = 0; i < sh_num; ++i) {
        tmp = &sh_name[shdr[i].sh_name];
        if (!strcmp(tmp, section_name)) {
            if (size)
                *size = shdr[i].sh_size;
            return f + shdr[i].sh_offset;
        }
    }

    fatal("can't find section \"%s\"\n", section_name);
    return NULL;
}