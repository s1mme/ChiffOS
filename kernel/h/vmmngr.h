#ifndef PAGING_H
#define PAGING_H
#include <types.h>
#define PAGESIZE            0x1000  // size
typedef struct page
{
	u32 present : 1;
	u32 rw : 1;
	u32 user : 1;
	u32 accessed : 1;
	u32 dirty : 1;
	u32 unused : 7;
	u32 frame_adress : 20; 
} page_t;

typedef struct page_table
{
page_t *pages[1024];	

}page_table_t;

typedef struct page_directory
{
	page_table_t *tables[1024];
	
	u32 table_phys[1024];
	
	//u32 phys_addr;
	
}page_directory_t;

 void _vmmngr_alloc_frame(page_t *page, int is_kernel, int is_writeable);
page_t *_vmm_get_page_addr(u32 addr, u32 make, page_directory_t  *dir);
u32 p_kmalloc(u32 size, u32 align, u32 *phys);


u32 paging_getPhysAddr(void* virtAddress);
void* paging_getVirtaddr(u32 physAddress, u32 numPages);


extern page_directory_t *kernel_directory;

#endif
