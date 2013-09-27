#ifndef HEAPMNGR_H
#define HEAPMNGR_H
#include <types.h>
#include <heapmngr.h>

#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x10000000
#define HEAP_INDEX_SIZE   0x200000
#define HEAP_MAGIC        0x123890AB
#define HEAP_MIN_SIZE     0x7000000
typedef void* type_t;

typedef struct 
{
	void **element;
	u32 size;
	u32 maxsize;

}table_t;


typedef struct
{
	u32 size;
	u8 is_hole;
	u32 magic;		
}desc_head;

typedef struct
{
	u32 magic;
	desc_head *header;
	
}desc_foot;

typedef struct
{
	table_t table;
	u32 maxaddr;
	u32 endaddr;
	u32 startaddr;
	u8 supervisor;
	u8 readonly;
}heap_t;

heap_t *_heapmngr_initialize( u32 heap_pool_start_pos, u32 heap_pool_end_pos,u32 sz);
void *kmalloc(u32 sz);
void free(void *ptr);

#endif
