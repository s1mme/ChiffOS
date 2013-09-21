#include <types.h>
u16 *video_ram = (u16 *)0xB8000;
int attrib = 0x0F;
void clear()
{
	int i;
	u16 blank = 0x20 | (attrib << 8);
	for(i = 0; i < 25; i++)
	{
	memsetw(video_ram + i*80, blank,80);
  }
}
