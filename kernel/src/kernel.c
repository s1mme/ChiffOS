#include <types.h>
#include <video.h>
#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <timer.h>
#include <multiboot.h>
#include <vmmngr.h>
#include <heapmngr.h>
void kmain(struct multiboot *mbp, u32 magic)
{
	
	  init_video_term();
	  gdt_install();
	  idt_install();
	  isrs_install();
	  irq_install();
	  timer_install(100);
	  _vmmngr_initialize(mbp);
	  __asm__ volatile("sti");
	  

	  for(;;);
}
