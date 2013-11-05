#ifndef ELF_H
#define ELF_H
#include <types.h>
#include <vmmngr.h>
typedef enum
{
    ET_NONE  = 0,
    ET_REL   = 1,
    ET_EXEC  = 2,
    ET_DYN   = 3,
    ET_CORE  = 4
} elf_header_type_t;

typedef enum
{
    EM_NONE  = 0,
    EM_M32   = 1,
    EM_SPARC = 2,
    EM_386   = 3,
    EM_86K   = 4,
    EM_88K   = 5,
    EM_860   = 7,
    EM_MIPS  = 8
} elf_header_machine_t;

typedef enum
{
    EV_NONE    = 0,
    EV_CURRENT = 1
} elf_header_version_t;

typedef enum
{
    EI_MAG0    = 0,
    EI_MAG1    = 1,
    EI_MAG2    = 2,
    EI_MAG3    = 3,
    EI_CLASS   = 4,
    EI_DATA    = 5,
    EI_VERSION = 6,
    EI_PAD     = 7
} elf_header_ident_t;

typedef enum
{
    ELFCLASSNONE = 0,
    ELFCLASS32   = 1,
    ELFCLASS64   = 2
} elf_header_ident_class_t;

typedef enum
{
    ELFDATANONE = 0,
    ELFDATA2LSB = 1,
    ELFDATA2MSB = 2
} elf_header_ident_data_t;


typedef enum
{
    PT_NULL = 0,
    PT_LOAD,
    PT_DYNAMIC,
    PT_INTERP,
    PT_NOTE,
    PT_SHLIB,
    PT_PHDR
} program_header_flags_t;
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
elf_header_t* parse_elf(void *elf_program_buf,u32 elf_file_size);
extern const elf_header_t * elf_header;
extern page_directory_t* pd;
#endif
