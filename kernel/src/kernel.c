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
u32 tmp;
u32 tmp2;
unsigned char *videoram = (unsigned char *)0xb8000;
extern u32 placement_address;
DIR *fs_root; 
void kmain(struct multiboot *mbp, u32 magic)
{
u32 initrd_location = *((u32*)mbp->mods_addr); //get the adress of initrd module
u32 initrd_end = *(u32*)(mbp->mods_addr+4); 
placement_address = initrd_end; 
init_video_term();
 puts("Welcome to chiffOS");
 printk("\n");
 
videoram[1] = 2;
videoram[3] = 4;
videoram[5] = 6;
videoram[7] = 8;
videoram[9] = 10;
videoram[11] = 12;
videoram[13] = 14;
videoram[15] = 12;
videoram[17] = 10;
videoram[19] = 8;
videoram[21] = 6;
videoram[23] = 4;
videoram[25] = 2;
videoram[27] = 6;
videoram[29] = 10;
videoram[31] = 14;

 
 printk("Initializing GDTs... ");
 gdt_install();
 printk("[ok]\n");
  
 printk("Initializing IDTs... ");
 idt_install();
 printk("[ok]\n");
 
 printk("Initializing ISRs and enabling interrupts... ");
 isrs_install();
 irq_install();
 printk("[ok]\n");
 
 printk("\n");
 printk("Initializing the PIT... ");
 timer_install(100);		
 printk("[ok]\n");
 
 printk("Initializing paging and the heap... ");
 _vmmngr_initialize(mbp); //get memory amount from grub
 printk("[ok]\n");
  
 printk("Setting up the keyboard handler... ");
 _kbd_initialize();
 printk("[ok]\n");
 
 printk("Initializing initrd... ");
 fs_root = install_initrd(initrd_location);
 printk("[ok]\n");

 
 printk("Initializing tasking... ");
 _task_initialize();
 printk("[ok]\n");
 
 _install_syscall_handler();
	
 scan_pci_bus(0);		
	
 ata_init_and_detect_drives();
 
 mount_fat32();
 fpu_install();
 FAT_shell_newlib();

for(;;);

}
