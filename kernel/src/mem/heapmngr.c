#include <types.h>
#include <video.h>
#include <video.h>
#include <kutils.h>
#include <heapmngr.h>
#include <vmmngr.h>

void *_heapmngr_get_element(u32 i, table_t *table);
heap_t * heap;
table_t _heapmngr_table_initialize(u32 sz, void *addr)
{
	table_t table;
	table.element = addr;	
	table.maxsize = sz;
	return table;	
}

void _heapmngr_insert_element(void *element, table_t *table )
{
	table->element = element;
}

u32 _heapmngr_find_element(u32 sz, heap_t *heap)
{
	u32 i = 0;
	while(i < heap->table.size)
	{
		header_t *header = _heapmngr_get_element(i, &heap->table);
			
		if(header->size >= sz)
			break;
		i++;
	}
	if(i == heap->table.size)
		return -1;
	else
		return i;
}

void* _heapmngr_get_element(u32 i, table_t *table)
{
	
	return &table->element[i];
	
}

heap_t *_heapmngr_initialize(u32 heap_pool_start_pos, u32 heap_pool_end_pos, u32 sz)
{
	 heap = (heap_t*)p_kmalloc(sizeof(heap_t),0,NULL);
	 memset(heap, 0, sizeof(heap_t));
	 
	 heap->table = _heapmngr_table_initialize(sz,(void*)heap_pool_start_pos);
			
	 header_t *hole = (header_t*)heap_pool_start_pos;
	 hole->is_hole = 1;
	 hole->size = heap_pool_end_pos - heap_pool_start_pos;
	 hole->magic = HEAP_MAGIC;
	 
	 _heapmngr_insert_element((void*)hole, &heap->table);
	 return heap;
}

void *request_kmalloc(u32 sz, u32 align, heap_t *heap)
{
	header_t *adress;
	u32 idx = _heapmngr_find_element(sz, heap);
	adress = _heapmngr_get_element(idx, &heap->table);
	
	
	return adress;
}

void release_kmalloc(void *ptr, heap_t *heap)
{
	if(ptr == 0)
	return;
	header_t *header = (header_t*) ( (u32)ptr - sizeof(header_t) );
	kprint("magic: %x", header->magic);
}

void *kmalloc(u32 sz)
{
	return request_kmalloc(sz, 1, heap);
}

void free(void *ptr)
{
	release_kmalloc(ptr, heap);
}
