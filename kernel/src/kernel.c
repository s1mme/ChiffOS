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
#include <kbd.h>
#include <fat.h>
#include <proc.h>
void kmain(struct multiboot *mbp, u32 magic)
{
	  init_video_term();
	  gdt_install();
	  idt_install();
	  isrs_install();
	  irq_install();
	  timer_install(60);
	  _vmmngr_initialize(mbp);
	  _kbd_initialize();
	  __asm__ volatile("sti");
	  
	  u32 a = kmalloc(8);
	  u32 b = kmalloc(8);
	  kprint("a: %x\n", a);
	  kprint("b: %x\n", b);
	
	  free(b);
	  free(a);
	  u32 c = kmalloc(12);
	  kprint("c: %x\n", c);
	  free(c);
	  u32 d = kmalloc(20);
	  kprint("d: %x\n", d);
	  free(d);
	  scan_pci_bus(0);
	  ata_init_and_detect_drives();
	  _install_syscall_handler();
	 /* write_disc_sector();*/ 
	  /*read_disc_sector(1);*/
	  _task_initialize();
					
	  TASK_testing();
	  mount_fat32();
	  FAT_testing();	  
	 
	  
	  while(1)
	  {
		  kputch(getchar());
	  }
	/*  u32 *ptr = (u32*)0x600000;
	  u32 do_page_fault = *ptr;*/
	  for(;;);
}
