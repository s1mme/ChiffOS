#include <types.h>
#include <video.h>
#include <kutils.h>
#include <heapmngr.h>
#include <vmmngr.h>
/*heap implementation with minimal error-checking*/

void *_heapmngr_get_element(u32 i, table_t *table);
extern heap_t * heap;
table_t _heapmngr_table_initialize(u32 sz, void *addr)
{
	table_t table;
	table.element = addr;
	memset(table.element, 0 , sizeof(table_t));	
	table.maxsize = sz;
	return table;	
}

void _heapmngr_insert_element(void *element, table_t *table )
{
	u32 iterator = 0;
	while(iterator < table->size && element < table->element)
	iterator++;
	if(iterator == table->size)
	table->element[table->size++] = element;
	else
	{
		void *tmp = table->element[iterator];
		table->element[iterator] = element;
		while(iterator < table->size)
		{
			iterator++;
			void *tmp2 = table->element[iterator];
		table->element[iterator] = tmp;
		tmp = tmp2;
		}
		table->size++;
	}
}

s32 _heapmngr_find_element(u32 sz, heap_t *heap)
{
	u32 i = 0;
	while(i < heap->table.size)
{
	desc_head *header = _heapmngr_get_element(i, &heap->table);

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
	
	heap_pool_start_pos += sizeof(type_t)*sz;
	
	desc_head *hole = (desc_head*)heap_pool_start_pos;
	hole->is_hole = 0;
	hole->size = heap_pool_end_pos - heap_pool_start_pos;
	hole->magic = HEAP_MAGIC;

	_heapmngr_insert_element((void*)hole, &heap->table);
	return heap;
}

void *request_kmalloc(u32 sz, u32 align, heap_t *heap)
{
	desc_head *old_element;
	u32 new_size = sz + sizeof(desc_head) + sizeof(desc_foot);
	s32 location = _heapmngr_find_element(new_size, heap);
	
	old_element = _heapmngr_get_element(location, &heap->table);
	
#ifdef DEBUGGING
	kprint("location : %d\n", location);
	kprint("Magic : %x\n", old_element->magic);
#endif
	u32 element_backup = (u32)old_element;
	u32 element_backup_size = old_element->size;
	
	_heapmngr_delete_element(location, &heap->table); /*delete current element*/
	
	old_element->size = new_size;
	
	desc_head *new_element = (desc_head *)(element_backup +  sz + sizeof(desc_head) + sizeof(desc_foot));
	new_element->size = element_backup_size - new_size;
	
	_heapmngr_insert_element((void*)new_element,&heap->table); /*add a new element*/
	
	return (void *)((u32)element_backup + sizeof(desc_head)); 
}

void release_kmalloc(void *ptr, heap_t *heap)
{
	if(ptr == 0)
	return;
	desc_head *element_old_head = (desc_head*) ( (u32)ptr - sizeof(desc_head) );

    desc_foot *element_old_foot = (desc_foot*) ( (u32)element_old_head + element_old_head->size - sizeof(desc_foot) );

 
    desc_head *test = (desc_head*) ( (u32)element_old_foot + sizeof(desc_foot) );

    element_old_head->size += test->size;     
    u32 i = 0;
    while ( (i < heap->table.size) &&
          (_heapmngr_get_element(i, &heap->table) != (void*)test) )
              i++;
              if (i < heap->table.size)
				_heapmngr_delete_element(i, &heap->table);
  
	_heapmngr_insert_element((void*)element_old_head, &heap->table);
}

void *kmalloc(u32 sz)
{
	return request_kmalloc(sz, 1, heap);
}

void free(void *ptr)
{
	release_kmalloc(ptr, heap);
}
