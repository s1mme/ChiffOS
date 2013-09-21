#include <types.h>

u16 *memsetw(u16 *dest, u16 value, int count)
{
	u16 *temp = (u16 *)dest;
	for( ;count != 0; count--) *temp++ = value;
	
	return dest;	
}
