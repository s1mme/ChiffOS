#include <types.h>
#include <irq.h>
#include <video.h>
#include <kutils.h>
#include <proc.h>
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
	/*if(current_task->time_to_run > 2) 
	{
		

	current_task->time_to_run--;
		

}
	else if(current_task->time_to_run < 2)
	{*/
	task_switching = true;
	
/*	}*/
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

int sleep(u32 ms) { 
	
    u32 t = ms/10;
   
    if (t == 0)t = 1;
	u32 start_ticks = gettickcount();
	while(start_ticks = gettickcount() < start_ticks + t) {}
	
	return start_ticks;
}
