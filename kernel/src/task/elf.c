#include <types.h>
#include <elf.h>
#include <vmmngr.h>
#include <video.h>
#include <kutils.h>
#include <heapmngr.h>
#include <proc.h>
struct p_directory* pd = 0;
#define ELF_START 0x1
#define ELF_END 0x1790000
const elf_header_t * elf_header;
elf_header_t* parse_elf(void *elf_program_buf,u32 elf_file_size)
{
	const u8 *elf_start = elf_program_buf;
	const u8 *elf_end = elf_start + elf_file_size;
	elf_header = (elf_header_t *)elf_program_buf;
	kprint("IDENT: %s\n", elf_header->ident);
	kprint("TYPE: %x\n", elf_header->type);
	kprint("MACHINE %x\n", elf_header->machine);
	kprint("VERSION %x\n", elf_header->version);
 
	pd = (struct p_directory*)p_kmalloc(sizeof(struct p_directory),1,NULL);
    memset(pd, 0, sizeof(struct p_directory));
	memcpy(pd, pkdirectory, sizeof(struct p_directory));
	
    int i;
      	 
/*	for (i = 0; i < ELF_START +  ELF_END; i += 0x1000)
		_vmm_get_page_addr(i, 1, pd);
	 
    for (i = 0; i < ELF_START +  ELF_END; i += 0x1000)
       _vmmngr_alloc_frame( _vmm_get_page_addr(i, 1, pd), 0, 1);
  */  
    u8* header_pos = elf_start + elf_header->phoff;
   
    if (header_pos+sizeof(program_header_t) >= elf_end)
    {
		return -1;
    }

    program_header_t* ph = (program_header_t*)header_pos;

	const char* types[] = { "NULL", "Loadable Segment", "Dynamic Linking Information",
                                "Interpreter", "Note", "??", "Program Header" };
    kprint(" %s\n offset: %x\n vaddr: %x\n paddr: %x\n filesz: %x\n memsz: %d\n flags: %x\n align: %x\n",
    types[ph->type], ph->offset, ph->vaddr, ph->paddr, ph->filesz, ph->memsz, ph->flags, ph->align);
    
    __asm__ __volatile__ ("cli");
    
    _vmmngr_switch_directory(pd);

    memset((void*)ph->vaddr, 0, ph->filesz); 

    memcpy((void*)ph->vaddr, elf_start+0x1000, ph->filesz);
      
    _vmmngr_switch_directory(pkdirectory);
        
    __asm__ __volatile__ ("sti");

    header_pos += elf_header->phentrysize;
         
   /* if(strcmp("run", programName) == 0)
    {*/		
    kprint("Program entry: %x\n", elf_header->entry);
    
    /*create_process((void*)elf_header->entry,PRIO_HIGH,0, 0);  */  /*argc and argv*/ 
	return elf_header;
       
    /*}*/
}


