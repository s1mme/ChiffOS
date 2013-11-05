#include <types.h>
#include <elf.h>
#include <vmmngr.h>
#include <video.h>
#include <kutils.h>
#include <heapmngr.h>
#include <proc.h>

page_directory_t* pd = 0;

void* globalUserProgAddr; 
uint32_t globalUserProgSize;
uint32_t alignUp(uint32_t val, uint32_t alignment)
{
    if (! alignment)
        return val;
    --alignment;
    return (val+alignment) & ~alignment;
}

elf_header_t *elf_exec(const void* elf_program_buf, uint32_t elf_file_size, const char* programName,size_t argc, char** argv)
{
    const u8 *elf_start = elf_program_buf;
	const u8 *elf_end = elf_start + elf_file_size;
	const elf_header_t* elf_header = (elf_header_t *)elf_program_buf;
	printk("IDENT: %s\n", elf_header->ident);
	printk("TYPE: %x\n", elf_header->type);
	printk("MACHINE %x\n", elf_header->machine);
	printk("VERSION %x\n", elf_header->version);
 
	pd = (page_directory_t*)malloc_a(sizeof(page_directory_t));
    memset(pd, 0, sizeof(page_directory_t));
	memcpy(pd, kernel_directory, sizeof(page_directory_t));
	u32 adress;
    adress = paging_getPhysAddr(pd);
    
	u32 test2;
	test2 = adress + 0x1000;
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
    printk(" %s\n offset: %x\n vaddr: %x\n paddr: %x\n filesz: %x\n memsz: %d\n flags: %x\n align: %x\n",
    types[ph->type], ph->offset, ph->vaddr, ph->paddr, ph->filesz, ph->memsz, ph->flags, ph->align);
    
    globalUserProgAddr = (void*)(ph->vaddr);
    globalUserProgSize = alignUp(ph->memsz,PAGESIZE);
    __asm__ __volatile__ ("cli");
    
   //switch_pagedirectory2(test2);

    memset((void*)ph->vaddr, 0, ph->filesz); 

    memcpy((void*)ph->vaddr, elf_start+ph->offset, ph->filesz);
      
    _vmmngr_switch_directory(kernel_directory);
        
    __asm__ __volatile__ ("sti");

    header_pos += elf_header->phentrysize;
     if(strcmp("run", programName) == 0)
    {
        create_process( (void*)elf_header->entry,THREAD, PRIO_IDLE,argc, argv);     
    }    
   		
    printk("Program entry: %x\n", elf_header->entry);
    
	return elf_header;
}
