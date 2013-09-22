#include <types.h>
#include <stddef.h>
#include <stdint.h>
#include <video.h>
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
u16 *video_ram = (u16 *)0xB8000;
u8 term_color;
int c_y;
int c_x;
enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};
u8 make_color(enum vga_color bg, enum vga_color fg)
{
	return fg | bg << 4;
}

u16 make_vga_entry(char c, u8 color)
{
	return c | color << 8;
}

void clear()
{
	int i;
	for(i = 0; i < VGA_HEIGHT; i++)
	{
	memsetw(video_ram + i*VGA_WIDTH, make_vga_entry(' ',make_color(COLOR_BLACK,COLOR_BLACK)),VGA_WIDTH);
    }
}

void set_term_color(u8 color)
{
term_color = color;
}

init_video_term()
{
	clear();
	c_y = 0;
	c_x = 0;
	set_term_color(make_color(COLOR_BLACK,COLOR_WHITE));
}
void update_cursor()
{
	u16 curloc = c_y * 80 + c_x; 
	outb(0x3D4, 14);
	outb(0x3D5, curloc >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, curloc);	
}
void scroll(void)
{
    if(c_y >= VGA_HEIGHT)
    {
        memcpy (video_ram, video_ram +  VGA_WIDTH, VGA_HEIGHT * VGA_WIDTH * 2);
        memsetw (video_ram + VGA_HEIGHT * VGA_WIDTH, make_vga_entry(' ',make_color(COLOR_BLACK,COLOR_BLACK)), VGA_WIDTH);
        c_y = VGA_HEIGHT - 1;
    }
}
void kputch(char c)
{
	u16 *where;
	if(c >= ' ')
	{
		where = video_ram + (c_y * 80 + c_x);
		*where = make_vga_entry(c,term_color);
		c_x++;
	}	
	else if(c == 0x08)
	{
		if(c_x != 0)c_x--;
	}
	else if(c_x >= 80)
	{
		c_x = 0;
		c_y++;
	}
	else if(c == 0x09)
	{
		c_x = (c_x + 8) &~(8-1);
	}
	else if(c == '\n')
	{	
		c_x = 0;
		c_y++;
	}
	else if(c == '\r')
	{
		c_x = 0;		
	}
	scroll();
	update_cursor();
}

void kputs(char *text)
{
	int i = 0;
	while(i < strlen(text))
		kputch(text[i++]);
}

void vprintf(const char *args, va_list ap)
{
	while(*args)
		{
			switch(*args)
			{
			case '%':
				switch(*(++args))
					{
    					case 'x':
							kputhex(va_arg(ap, u32));
						break;
						case 'd':
							kputint(va_arg(ap, s32));
						break;
						case 's':
							kputs(va_arg(ap, char*));
						break;					
						default:
						break;
					}								
			break;
			default:
			break;
			}			
		args++; 	
		}	
}

void kprint(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
    update_cursor();
}