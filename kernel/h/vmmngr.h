#ifndef PAGING_H
#define PAGING_H
#include <types.h>
void _vmmngr_initialize(struct multiboot *mbp);

struct p_pages
{
   u32 present    : 1;   
   u32 rw         : 1;   
   u32 user       : 1;  
   u32 accessed   : 1;   
   u32 dirty      : 1;   
   u32 unused     : 7;   
   u32 frame_addr      : 20;
}__attribute__((packed));

struct p_tables
{
	struct p_pages pages[1024];
}__attribute__((packed));

struct p_directory
{
	struct p_tables *tables[1024];
	u32 tablephysical[1024];
}__attribute__((packed));

void _vmmngr_alloc_frame(struct p_pages *page, int is_kernel, int is_writeable);
struct p_pages *_vmm_get_page_addr(u32 addr, u32 make, struct p_directory *dir);
u32 p_kmalloc(u32 size, u32 align, u32 *phys);

#endif
