#ifndef ELF_H
#define ELF_H
#include <types.h>
typedef struct
{
    u8  ident[16];
    u16 type;
    u16 machine;
    u32 version;
    u32 entry;
    u32 phoff;
    u32 shoff;
    u32 flags;
    u16 ehsize;
    u16 phentrysize;
    u16 phnum;
    u16 shentrysize;
    u16 shnum;
    u16 shstrndx;
} elf_header_t;


typedef struct
{
    u32 type;
    u32 offset;
    u32 vaddr;
    u32 paddr;
    u32 filesz;
    u32 memsz;
    u32 flags;
    u32 align;
} program_header_t;
bool parse_elf(void *elf_program_buf,u32 elf_file_size);
extern const elf_header_t * elf_header;
#endif
