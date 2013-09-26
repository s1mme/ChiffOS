#include <types.h>
#include <video.h>
#include <video.h>
#include <kutils.h>
#include <heapmngr.h>
#include <vmmngr.h>

void *_heapmngr_get_element(u32 i, table_t *table);
extern heap_t * heap;
table_t _heapmngr_table_initialize(u32 sz, void *addr)
{
	table_t table;
	table.element = addr;	
	table.maxsize = sz;
	return table;	
}

void _heapmngr_insert_element(void *element, table_t *table )
{
	u32 i = 0;
	while (i < table->size && element > table->element)
       i++;
       
    if (i == table->size) 
       table->element[table->size++] = element;
}

s32 _heapmngr_find_element(u32 sz, heap_t *heap)
{
	u32 i = 0;
	while(i < heap->table.size)
{
	element_desc_head *header = _heapmngr_get_element(i, &heap->table);

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
	return table->element[i];
}

void _heapmngr_delete_element(u32 i, table_t *table)
{
   while (i < table->size)
   {
       table->element[i] = table->element[i+1];
       i++;
   }
   table->size--;
} 


heap_t *_heapmngr_initialize(u32 heap_pool_start_pos, u32 heap_pool_end_pos, u32 sz)
{
	heap = (heap_t*)p_kmalloc(sizeof(heap_t),1,0);
	memset(heap, 0, sizeof(heap_t));

	heap->table = _heapmngr_table_initialize(sz,(void*)heap_pool_start_pos);
	
	heap_pool_start_pos += sizeof(type_t)*HEAP_INDEX_SIZE;
	
	element_desc_head *hole = (element_desc_head*)heap_pool_start_pos;
	hole->is_hole = 0;
	hole->size = heap_pool_end_pos - heap_pool_start_pos;
	hole->magic = HEAP_MAGIC;

	_heapmngr_insert_element((void*)hole, &heap->table);
	return heap;
}

void *request_kmalloc(u32 sz, u32 align, heap_t *heap)
{
	element_desc_head *old_element;
	u32 new_size = sz + sizeof(element_desc_head) + sizeof(element_desc_foot);
	s32 location = _heapmngr_find_element(sz, heap);
	old_element = _heapmngr_get_element(location, &heap->table);
	u32 element_backup = (u32)old_element;
	u32 element_backup_size = old_element->size;
	
	_heapmngr_delete_element(location, &heap->table); /*delete current element*/
	
	old_element->size = new_size;
	
	element_desc_head *new_element = (element_desc_head *)(element_backup +  sz + sizeof(element_desc_head) + sizeof(element_desc_foot));
	new_element->size = element_backup_size - new_size;
	
	_heapmngr_insert_element((void*)new_element,&heap->table); /*add a new element*/
	
	return (void *)((u32)element_backup + sizeof(element_desc_head)); ;
}

void release_kmalloc(void *ptr, heap_t *heap)
{
	if(ptr == 0)
	return;
	element_desc_head *old_head = (element_desc_head*) ( (u32)ptr - sizeof(element_desc_head) );

    element_desc_foot *old_foot = (element_desc_foot*) ( (u32)old_head + old_head->size - sizeof(element_desc_foot) );

 
    element_desc_head *test = (element_desc_head*) ( (u32)old_foot + sizeof(element_desc_foot) );

    old_head->size += test->size;     
    u32 i = 0;
    while ( (i < heap->table.size) &&
          (_heapmngr_get_element(i, &heap->table) != (void*)test) )
              i++;
              if (i < heap->table.size)
					_heapmngr_delete_element(i, &heap->table);
  
	_heapmngr_insert_element((void*)old_head, &heap->table);
}

void *kmalloc(u32 sz)
{
	return request_kmalloc(sz, 1, heap);
}

void free(void *ptr)
{
	release_kmalloc(ptr, heap);
}
