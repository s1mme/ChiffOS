#include <types.h>
#include <video.h>
#include <kutils.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
u16 *video_ram = (u16 *)0xB8000;
u8 term_color;
u32 c_y;
u32 c_x;

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
	u32 i;
	for(i = 0; i < VGA_HEIGHT; i++)
	{
	memsetw(video_ram + i*VGA_WIDTH, make_vga_entry(' ',make_color(COLOR_BLACK,COLOR_BLACK)),VGA_WIDTH);
    }
}

void set_term_color(u8 color)
{
term_color = color;
}

void init_video_term()
{
	clear();
	c_y = 0;
	c_x = 0;
	set_term_color(make_color(COLOR_BLACK,COLOR_WHITE));
}
void update_cursor(void)
{
	u16 curloc = c_y * 80 + c_x; 
	outb(0x3D4, 14);
	outb(0x3D5, curloc >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, curloc);	
}
void scroll(void)
{
    if(c_y >= 25)
    {
        memcpy (video_ram, video_ram +  80, 24 * 80 * 2);
        memsetw (video_ram + 24 * 80, make_vga_entry(' ',make_color(COLOR_BLACK,COLOR_BLACK)), 80);
        c_y = 25 - 1;
    }
}

void putch(char c)
{
	u16 *where;
	 if(c == 0x08)
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
	
	else if(c >= ' ')
	{
		where = video_ram + (c_y * 80 + c_x);
		*where = make_vga_entry(c,term_color);
		c_x++;
		
	}	
	scroll();
	update_cursor();
}

void puts(const char *text)
{
	u32 i = 0;
	while(i < strlen(text))
		putch(text[i++]);
		
}

void vprintf(const char *args, va_list ap)
{
	char buffer[100];
	while(*args)
		{
			switch(*args)
			{
			case '%':
				switch(*(++args))
					{
						case 'c':
							 putch((s8)va_arg(ap, s32));
							 break;
    					case 'x':
							monitor_write_hex(va_arg(ap, u32));
						break;
						case 'd':
							put_dec(va_arg(ap, s32));
						break;
						case 's':
							puts(va_arg(ap, char*));
						break;					
						default:
						--args;
						break;
					}								
			break;
			default:
			putch(*args);
			break;
			}			
		args++; 	
		}	
}

int stdio_read(int fd, void *buf, int length) {
	 
	 if(fd == 1)
		{
			buf = getchar();
			return 1;
		}
char *p = (char *)buf;



int ret = 0;
while (p < (char *)buf + 20 - 1 /* NULL termination */) {

char c = getchar();

if (c >= ' ' || c == '\n') {
putch(c); 
update_cursor();
}
else if (c == '\b') {
	
if (p > (char *)buf) {
p--;
putch(c);
putch(' ');
putch(c);
update_cursor();
ret--;
}
}
else if (c == -1) {

if (ret > 0)
continue;
else {
putch('^');
putch('D');
return 0;
}
}


if (c == '\r' || c == '\n') {
ret++;
*p++ = c;
*p = 0;
return ret;
}
else if (c != '\b') {
*p++ = c;
ret++;
}
}


*p = 0;

return ret;
}

void printk(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
    update_cursor();
    
}

void put_dec(u32 n)
{

    if (n == 0)
    {
        putch('0');
        return;
    }

    s32 acc = n;
    char c[32];
    int i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc%10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    int j = 0;
    while(i >= 0)
    {
        c2[i--] = c[j++];
    }
    puts(c2);

}

void monitor_write_hex(u32 n)
{
    s32 tmp;

    puts("0x");

    char noZeroes = 1;

    int i;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && noZeroes != 0)
        {
            continue;
        }
    
        if (tmp >= 0xA)
        {
            noZeroes = 0;
            putch(tmp-0xA+'a' );
        }
        else
        {
            noZeroes = 0;
            putch( tmp+'0' );
        }
    }
  
}
/// http://en.wikipedia.org/wiki/Itoa
void reverse(char* s)
{
    char c;

    for (s32 i=0, j=strlen(s)-1; i<j; i++, j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

s8 ctoi(char c) {
    if (c < 48 || c > 57) {
        return(-1);
    }
    return(c-48);
}
char* utoa(u32 n, char* s)
{
    u32 i = 0;
    do 
    {
        s[i++] = n % 10 + '0'; 
    }
    while ((n /= 10) > 0);     
    s[i] = '\0';
    reverse(s);
    return(s);
}

char* itoa(s32 n, char* s)
{
    bool sign = n < 0;
    if (sign)   
    {
        n = -n;
    }
    u32 i = 0;
    do 
    {
        s[i++] = n % 10 + '0'; 
    }
    while ((n /= 10) > 0);     

    if (sign)
    {
        s[i++] = '-';
    }
    s[i] = '\0';
    reverse(s);
    return(s);
}

void i2hex(u32 val, char* dest, u32 len)
{
    char* cp = &dest[len];
    while (cp > dest)
    {
        char x = val & 0xF;
        val >>= 4;
        *--cp = x + ((x >= 9) ? 'A' - 10 : '0');
    }
    dest[len]='\0';
}
