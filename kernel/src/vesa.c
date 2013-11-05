#include <fat.h>
#include <video.h>
#include <types.h>
#include <heapmngr.h>
#include <kutils.h>
#include <ata.h>
#include <elf.h>
#include <proc.h>
#include <fat.h>
#include <vesa.h>
#include <vmmngr.h>
#include <vesa.h>
u16* DOUBLEBUFFER_vbe;
VESA_MODE_INFO mib;
VGA_INFO  vgaIB;
unsigned int* SCREEN ; 
extern uintptr_t vesa_com_start;
extern uintptr_t vesa_com_end;

VESA_MODE_INFO* getCurrentMIB()
{
    return(&mib);
}

void vesa_VIB()
{
	FILE elf;
	elf_header_t * elf_program;
	elf = ls_dir("VGA",0);
	 *(char*)0x3400 = 'V';
    *(char*)0x3401 = 'B';
    *(char*)0x3402 = 'E';
    *(char*)0x3403 = '2';
    create_task_vm86(elf_program->entry,"vesa");
    sleep2(1000);
	memcpy(&vgaIB, (void*)0x3400, sizeof(VGA_INFO)); 
}

void vesa_MIB(u16 mode) 
{
	FILE elf;
	elf_header_t * elf_program;
	elf = ls_dir("VESA",0);
	elf_program = read_elf(elf);
    *(u16*)0x3600 = mode;
    create_task_vm86(elf_program->entry,"vesa");
    memcpy(&mib, (void*)0x3600, sizeof(VESA_MODE_INFO));
}




void setVideoMemory()
{
     u32 numberOfPages =0x620000 / 0x1000; 
     SCREEN = physicaltovirt(mib.PhysBasePtr, numberOfPages);
}

unsigned int * giveSCREEN()
{
	return SCREEN;
}
void gmode_switch(u16 mode)
{
	FILE elf;
	elf_header_t * elf_program;
	elf = ls_dir("GLOADER",0);
	
	elf_program = read_elf(elf);
    *(u16*)0x3600 = 0xC1FF&(0xC000|mode);
    create_task_vm86(elf_program->entry,"vesa");
        
    vesa_MIB(mode);
      
    setVideoMemory();
    if(!(mode&BIT(15))) 
        memset(SCREEN, 0, mib.XResolution*mib.YResolution*(mib.BitsPerPixel % 8 == 0 ? mib.BitsPerPixel/8 : mib.BitsPerPixel/8 + 1));   
}



 void VESAinfo()
{

    printk("VESAinfo:\n"); 
    printk("Video Memory :");
	printk("%d MB \n",vgaIB.TotalMemory/0x10);
    puts("Video Modes Ptr: ");  
	monitor_write_hex(vgaIB.VideoModes);
	puts("\n");

    for (u16 i=0; i < 256; i++)
    {	
        if(vgaIB.VideoModes[i] == 0xFFFF) break;      
//        vbe_readMIB(vgaIB.VideoModes[i]);
    }
}

void gDebug()
{ 
    printk("\nVgaInfoBlock:\n");
    printk("VESA-Signature:  %c%c%c%c\n", vgaIB.VESASignature[0], vgaIB.VESASignature[1], vgaIB.VESASignature[2], vgaIB.VESASignature[3]);
    printk("VESA-Version:    %x.%x\n",    vgaIB.VESAVersion>>8, vgaIB.VESAVersion&0xFF); 
    printk("Capabilities:    %xh\n",       vgaIB.Capabilities[0]);
    printk("Video Memory:    %d MiB\n",   vgaIB.TotalMemory/0x10); 
    printk("Video Modes Ptr: %xh\n",       vgaIB.VideoModes);
    printk("\nVideo Modes:");

    for (u16 i=0; i < 256; i++)
    {
		//getchar();
        if(vgaIB.VideoModes[i] == 325) break; 
		//    vbe_readMIB(vgaIB.VideoModes[i]);
        if(!(mib.ModeAttributes & BIT(0)) || !(mib.ModeAttributes & BIT(7))) continue;
        printk("\n%u (%xh) = %ux%ux%u", vgaIB.VideoModes[i], vgaIB.VideoModes[i], mib.XResolution, mib.YResolution, mib.BitsPerPixel);
        if(!(mib.ModeAttributes & BIT(4))) printk(" (textmode)");
    }
    printk("\n");
   
}

void _init_VESA(DIR *root_)
{
	__asm__ __volatile__ ("sti");	
    vesa_VIB();
    gDebug();
    VESAinfo();
    vesa_VIB(324); 
    gmode_switch(324);
}
