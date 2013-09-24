#include <types.h>
#include <irq.h>
#include <video.h>
#include <kutils.h>
volatile u32 timerticks = 0;
	
u32 uptime()
{
	return timerticks/100;
}

void timer_div(int hz)
{
	u16 divisor = 1193180 / hz;
	
	outb(0x43, 0x36);
	outb(0x40, divisor & 0xff);
	outb(0x40, divisor >> 8);	
}
 u32 timer_handle(u32 esp)
{
	timerticks++;
	/*kprint("%d", timerticks);*/
	return esp;	
}
   
         
 u32 gettickcount(void) {

	return timerticks;
}
     
	
void timer_install(u32 frequency)
{
	
	timer_div(frequency);
	register_device(0,timer_handle);
	
}
