#include <types.h>
#include <video.h>
#include <multiboot.h>
#include <vmmngr.h>
#include <video.h>
#include <kutils.h>
#include <irq.h>
#include <heapmngr.h>

u32 adress;
page_directory_t *kernel_directory;
u32 p_kmalloc(u32 size, u32 align, u32 *phys);
extern u32 end;
void page_fault(struct regs *r);
page_directory_t *clone_directory(page_directory_t *src);
u32 vmm_get_phys(u32 virtual, page_directory_t *dir);
u32 num_pages;
heap_t *kheap;
u32 *frames;
u32 placement_address = (u32)&end; //from linker script

#define PAGE_SIZE 0x1000
u32 paging_getPhysAddr(void* virtAddress)
{
    page_directory_t* pd = kernel_directory;


    u32 pagenr = (u32)virtAddress / PAGE_SIZE;
    page_table_t* pt = pd->tables[pagenr/1024];

    return ((u32)&pt->pages[pagenr%1024]&0xFFFF000) + (((u32)virtAddress)&0x00000FFF);
}
#define MEM_START (u8*)0x0a0000000
void* paging_getVirtaddr(u32 physAddress, u32 numPages)
{
	u32 i;
    static u8* virtAddress = MEM_START; 
    for (i=0; i<numPages; i++)
    {
        u32 pagenr = (u32)virtAddress/PAGE_SIZE;

        kernel_directory->tables[pagenr/1024]->pages[pagenr%1024] = physAddress | 1 | 2;

        virtAddress += PAGE_SIZE;
        physAddress += PAGE_SIZE;
    }
    return (void*)0xa0000000; 
}
 
 
u32 pagefault_handler(u32 esp)
{
	
   struct regs *r = (struct regs *)esp;
   u32 faulting_address;
   __asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));
   
   int present  = !(r->err_code & 0x1); 
   int rw = r->err_code & 0x2;           
   int us = r->err_code & 0x4;           
   int reserved = r->err_code & 0x8;     
         
  
   if(present)
		printk("Page not present! ");
   if(rw)
		printk("Read-only ");
   if(us)
		printk("User-mode ");
   if(reserved)
		printk("Reserved ");
   printk("faulting adress: %x\n", faulting_address);
   for(;;);
   return 0 ;
}

u32 _pmm_find_first_free_frame_addr()
{
	u32 i = 0;
	u32 j;
	u32 index = num_pages / 32;	
	while(i < index)
	{	
		if(frames[i] != 0xffffffff)
		{
			for(j = 0; j < 32; j++)
			{
				if(!(frames[i]&(0x1 << j)))
					return i*32+j;	/*address of free frame*/
			}		
		}	
	i++;
	}
	return 0;
}

page_t *_vmm_get_page_addr(u32 addr, u32 make, page_directory_t *dir)
{
	addr /= PAGE_SIZE;
	u32 table_index = addr / 1024;
	if(dir->tables[table_index])
		return (u32*)&dir->tables[table_index]->pages[addr%1024];
	else if(make)
		{
			u32 tmp;
			dir->tables[table_index] = (page_table_t*)p_kmalloc(sizeof(page_table_t),1,&tmp);
			memset(dir->tables[table_index], 0 , PAGE_SIZE);
			dir->table_phys[table_index]  = tmp | 0x7;
			return (u32*)&dir->tables[table_index]->pages[addr%1024];			
		}
	else
		return 0;
}

void _vmmngr_alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
	if(page->frame_adress != 0)
		return;
	else
		{	
			u32 idx = _pmm_find_first_free_frame_addr();
			if(idx == (u32)-1)
				printk("No free frames");
			page->frame_adress = idx;
			page->present = 1;
			page->user = (is_kernel)?0:1;
			page->rw = (is_writeable)?1:0;
			frames[idx/32] |= (0x1 << idx%32); 
		}
}

u32 p_kmalloc(u32 size, u32 align, u32 *phys)
{
	if(align == 1 && (placement_address & 0xfffff000))
		{
			placement_address &= 0xfffff000;
			placement_address += PAGE_SIZE;			
		}
	if (phys)
		{
			*phys = placement_address;
		}
	u32 tmp = placement_address;
	placement_address += size;
	return tmp;
}
#define VESA_START 0xa0000000
#define VESA_END   0xa1600000

#define ELF_START 0x1400000
#define ELF_END 0x1600000

void _vmmngr_switch_directory(page_directory_t *dir)
{
   __asm__ volatile("mov %0, %%cr3":: "r"(&dir->table_phys));
   u32 cr0;
   __asm__ volatile("mov %%cr0, %0": "=r"(cr0));
   cr0 |= 0x80000000; 
   __asm__ volatile("mov %0, %%cr0":: "r"(cr0));
}

void _vmmngr_initialize(struct multiboot *mbp)
{

	printk("[INFO] size of RAM: %d kb\n",mbp->mem_upper);
	u32 total_size = mbp->mem_upper * 1024;
	num_pages = total_size / PAGE_SIZE;
	printk("[INFO] amount of frames: %d\n",num_pages);
	frames = (u32*)p_kmalloc(num_pages/32,1,NULL);
	memset(frames, 0, num_pages/32);
	
	
	kernel_directory = (page_directory_t*)p_kmalloc(sizeof(page_directory_t),1,NULL);
	memset(kernel_directory,0,sizeof(page_directory_t));
	u32 i = 0;
	printk("[INFO] placement_address: %x\n", placement_address);

	
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += PAGE_SIZE)
		_vmm_get_page_addr(i, 1, kernel_directory);
	for (i = VESA_START; i < VESA_END; i += PAGE_SIZE) 
       _vmm_get_page_addr(i, 1, kernel_directory);

	for (i = ELF_START; i < 0x1400000 + 0x4000000; i += PAGE_SIZE)
      _vmm_get_page_addr(i, 1, kernel_directory);
      i = 0;
	while(i < 0x1390000)
	 {
		_vmmngr_alloc_frame(_vmm_get_page_addr(i,1,kernel_directory),0,1);
		i += PAGE_SIZE; 
	 }
	i = 0;
	for (i = ELF_START; i <  0x1400000 + 0x4000000; i += PAGE_SIZE)
		_vmmngr_alloc_frame( _vmm_get_page_addr(i, 1, kernel_directory), 0, 1);
		
	for (i = VESA_START; i <  VESA_END ; i += PAGE_SIZE)
	_vmmngr_alloc_frame( _vmm_get_page_addr(i, 1, kernel_directory), 0, 1);
		
		
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += PAGE_SIZE)
       _vmmngr_alloc_frame( _vmm_get_page_addr(i, 1, kernel_directory), 0, 1);

	install_device(14, pagefault_handler);
	
	_vmmngr_switch_directory(kernel_directory);
	kheap = _heapmngr_initialize(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, HEAP_INDEX_SIZE);
}


#define PCI_MEM_START ((u8*)0x0a0000000)
#define PCI_MEM_END ((u8*)0x100000000) 
enum MEM_FLAGS {MEM_KERNEL = 0, MEM_PRESENT = 1, MEM_WRITE = 2, MEM_USER = 4};
void* physicaltovirt(u32 physAddress, u32 numberOfPages)
{
    static u8* virtAddress = PCI_MEM_START;
    u8* retVal = 0;
    

    for (u32 i=0; i<numberOfPages; i++)
    {
        if (virtAddress == PCI_MEM_END)
        {
        
          puts("error");
       
        }

        u32 pagenr = (u32)virtAddress/PAGE_SIZE;

        kernel_directory->tables[pagenr/1024]->pages[pagenr%1024] = physAddress | MEM_PRESENT | MEM_WRITE;

        if (i==0)
        {
            retVal = virtAddress;
        }

        virtAddress += PAGE_SIZE;
        physAddress += PAGE_SIZE;
    }


    return (void*)retVal; 
}
