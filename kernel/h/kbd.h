#ifndef KBD_H
#define KBD_H
#include <types.h>

typedef struct ringbuffer {
 u8 data[128];
 u8 *read_ptr;
 u8 *write_ptr; /* volatile is probably not needed */
 u32 counter; /* how much unread data is stored? */
} ringbuffer_t;
void _kbd_initialize();
u8 getchar();
#endif
