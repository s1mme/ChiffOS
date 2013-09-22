#ifndef KUTIlS_H_
#define KUTIlS_H_
#include <types.h>
#include <stddef.h>
#include <stdint.h>
void outb(u16 port, u8 data);
size_t strlen(const char* str);
u16 *memsetw(u16 *dest, u16 value, int count);
void *memcpy(void *dest,const void *src,size_t n);
#endif