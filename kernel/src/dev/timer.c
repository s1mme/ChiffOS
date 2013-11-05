#include <types.h>
#include <irq.h>
#include <video.h>
#include <kutils.h>
#include <proc.h>

#define BCD2BIN(bcd) ((((bcd)&15) + ((bcd)>>4)*10))
const u16 TIMER_DIVISOR = 11932;
volatile u32 timerticks = 0;
volatile unsigned int useconds = 0;


volatile int hours;
volatile int min;
volatile int sec;
u32 sys_get_system_time()
{
	return timerticks/100;
}
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

if(current_task->time_to_run > 0) 
{
  current_task->time_to_run--;
}
else
   {
	task_switching = 1;

        }

	return esp;
	
	
    }
   
         
volatile u32 gettickcount(void) {

return timerticks;
}
 /*    
datetime_t getDatetime()
{
   datetime_t now;

   __asm__ __volatile__ ("cli");
   now.sec = BCD2BIN(readCMOS(0x0));
   now.min = BCD2BIN(readCMOS(0x2));
   now.hour = BCD2BIN(readCMOS(0x4));
   now.day = BCD2BIN(readCMOS(0x7));
   now.month = BCD2BIN(readCMOS(0x8));
   now.year = BCD2BIN(readCMOS(0x9));
  __asm__ __volatile__ ("sti");

   return now;
}
*/
int sleep(u32 ms) { 
	
   u32 t = ms/10;
   
    if (t == 0)t = 1;
	u32 start_ticks = gettickcount();
	while(start_ticks = gettickcount() < start_ticks + t) {}
	return start_ticks;
}



void timer_install(u32 frequency)
{
	  timer_div(frequency);
	  install_device(0,timer_handle);
}

void timedate()
{
int tmpx, tmpy;
	while(1)
	{/*	
		datetime_t t = getDatetime();
		c_x= 0;
		c_y = 0;
		printk("The time is now:  %d:%d:%d %d/%d/%d",t.hour, t.min, t.sec, t.day, t.month, t.year);*/
	}
	
}

