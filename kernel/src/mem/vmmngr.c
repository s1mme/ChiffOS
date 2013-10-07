#include <types.h>
#include <video.h>
#include <multiboot.h>
#include <vmmngr.h>
#include <video.h>
#include <kutils.h>
#include <irq.h>
#include <heapmngr.h>
#define DESCSIZE 32
#define PAGE_SIZE 0x1000
 heap_t *heap; 
u32 p_kmalloc(u32 size, u32 align, u32 *phys);
u32 *frames;
u32 num_pages;
extern u32 end;
u32 placement_address = (u32)&end;
struct p_directory *pkdirectory;
 
u32 pagefault_handler(u32 esp)
{
	
   struct regs *r = (struct regs *)esp;
   u32 faulting_address;
   __asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));
   
   int present  = !(r->err_code & 0x1); 
   int rw = r->err_code & 0x2;           
   int us = r->err_code & 0x4;           
   int reserved = r->err_code & 0x8;     
  /* int id = r->err_code & 0x10;*/         
  
   if(present)
		kprint("Page not present! ");
   if(rw)
		kprint("Read-only ");
   if(us)
		kprint("User-mode ");
   if(reserved)
		kprint("Reserved ");
   kprint("faulting adress: %x\n", faulting_address);
   for(;;);
   return 0 ;
}

u32 _pmm_find_first_free_frame_addr()
{
	u32 i = 0;
	u32 j;
	u32 index = num_pages / DESCSIZE;	
	while(i < index)
	{	
		if(frames[i] != 0xffffffff)
		{
			for(j = 0; j < DESCSIZE; j++)
			{
				if(!(frames[i]&(0x1 << j)))
					return i*DESCSIZE+j;	/*address of free frame*/
			}		
		}	
	i++;
	}
	return 0;
}

struct p_pages *_vmm_get_page_addr(u32 addr, u32 make, struct p_directory *dir)
{
	addr /= 0x1000;
	u32 table_index = addr / 1024;
	if(dir->tables[table_index])
		return (struct p_pages*)&dir->tables[table_index]->pages[addr%1024];
	else if(make)
		{
			u32 tmp;
			dir->tables[table_index] = (struct p_tables*)p_kmalloc(sizeof(struct p_tables),1,&tmp);
			memset(dir->tables[table_index], 0 , 0x1000);
			dir->tablephysical[table_index]  = tmp | 0x7;
			return (struct p_pages*)&dir->tables[table_index]->pages[addr%1024];			
		}
	else
		return 0;
}

void _vmmngr_alloc_frame(struct p_pages *page, int is_kernel, int is_writeable)
{
	if(page->frame_addr != 0)
		return;
	else
		{	
			u32 idx = _pmm_find_first_free_frame_addr();
			if(idx == (u32)-1)
				kprint("No free frames");
			page->frame_addr = idx;
			page->present = 1;
			page->user = (is_kernel)?0:1;
			page->rw = (is_writeable)?1:0;
			frames[idx/32] |= (0x1 << idx%32); /*set the frame busy*/
		}
}

u32 p_kmalloc(u32 size, u32 align, u32 *phys)
{
	if(align == 1 && (placement_address & 0xfffff000))
		{
			placement_address &= 0xfffff000;
			placement_address += 0x1000;			
		}
	if (phys)
		{
			*phys = placement_address;
		}
	u32 tmp = placement_address;
	placement_address += size;
	return tmp;
}

void _vmmngr_switch_directory(struct p_directory *dir)
{
   __asm__ volatile("mov %0, %%cr3":: "r"(&dir->tablephysical));
   u32 cr0;
   __asm__ volatile("mov %%cr0, %0": "=r"(cr0));
   cr0 |= 0x80000000; 
   __asm__ volatile("mov %0, %%cr0":: "r"(cr0));
}

void _vmmngr_initialize(struct multiboot *mbp)
{

	kprint("[INFO] size of RAM: %d kb\n",mbp->mem_upper);
	u32 total_size = mbp->mem_upper * 1024;
	num_pages = total_size / 0x1000;
	kprint("[INFO] amount of frames: %d\n",num_pages);
	frames = (u32*)p_kmalloc(num_pages/DESCSIZE,0,NULL);
	memset(frames, 0, num_pages/DESCSIZE);
	
	
	pkdirectory = (struct p_directory*)p_kmalloc(sizeof(struct p_directory),1,NULL);
	memset(pkdirectory,0,sizeof(struct p_directory));
	
	kprint("[INFO] placement_address: %x\n", placement_address);
	
	u32 i = 0;
	while(i < placement_address+0x6000000)
	 {
		_vmmngr_alloc_frame(_vmm_get_page_addr(i,1,pkdirectory),0,1);
		i += 0x1000; 
	}
	 i = 0;
   for (i = 0x10b000; i < 0x10b000+0x40b0000; i += 0x1000)
      _vmm_get_page_addr(i, 1, pkdirectory);
	 
   for (i = 0x10b000; i < 0x10b000+0x40b0000; i += 0x1000)
       _vmmngr_alloc_frame( _vmm_get_page_addr(i, 1, pkdirectory), 0, 1);
	
	 
   for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
       _vmm_get_page_addr(i, 1, pkdirectory);

   for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
       _vmmngr_alloc_frame( _vmm_get_page_addr(i, 1, pkdirectory), 0, 1);
	
	
	
	register_device(14, pagefault_handler);
	_vmmngr_switch_directory(pkdirectory);
	heap = _heapmngr_initialize(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, HEAP_INDEX_SIZE);
}

