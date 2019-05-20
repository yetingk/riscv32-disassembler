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


uint16_t section_idx(elf32 *e, const char *section_name)
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

    for (uint16_t i = 0; i < sh_num; ++i) {
        tmp = &sh_name[shdr[i].sh_name];
        if (!strcmp(tmp, section_name))
            return i;
    }

    fatal("can't find section \"%s\"\n", section_name);
    return -1;
}


void *find_section(elf32 *e, const char *section_name, ssize_t *size)
{
    uint16_t sh_idx = section_idx(e, section_name);
    if (sh_idx < 0)
        return NULL;

    if (size)
        *size = e->shdr[sh_idx].sh_size;
    return e->file + e->shdr[sh_idx].sh_offset;
}


/* to record symbol */
static SymList *sym_create(uint32_t offset, const char *sym_name)
{
    SymList *l = malloc(sizeof(SymList));
    l->offset = offset;
    l->name = sym_name;
    l->next = NULL;
    return l;
}

static void insert_symlist(SymList **head,
                           uint32_t offset,
                           const char *sym_name)
{
    SymList *new, **tmp;

    new = sym_create(offset, sym_name);

    tmp = head;
    for (; *tmp && offset > (*tmp)->offset; tmp = &(*tmp)->next)
        ;

    new->next = *tmp;
    *tmp = new;
}

SymList *func_list(elf32 *e)
{
    size_t size;
    uint16_t sh_num, text_shndx;
    const char *name;
    Elf32_Sym *sym;

    text_shndx = section_idx(e, ".text");
    name = find_section(e, ".strtab", NULL);
    sym = find_section(e, ".symtab", &size);
    sh_num = size / sizeof(Elf32_Sym);

    SymList *head = NULL;

    for (int i = 0; i < sh_num; ++i) {
        if (ELF32_ST_TYPE(sym[i].st_info) == STT_FUNC &&
            sym[i].st_shndx == text_shndx)
            insert_symlist(&head, sym[i].st_value, &name[sym[i].st_name]);
    }
    return head;
}

void free_symlist(SymList *l)
{
    while (l) {
        SymList *tmp = l->next;
        free(l);
        l = tmp;
    }
}
