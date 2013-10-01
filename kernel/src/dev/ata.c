#include <types.h>
#include <video.h>
#include <kutils.h>
#include <irq.h>
#include <ata.h>

u8 buffer[2048] = {0};

u32 ata_irq_counts = 0;
u32 ata_interrupt_handler(u32 esp);

void ide_cntrl_write_reg(u8 channel, u16 reg, u8 data)
{
	if (reg == ATA_REG_CONTROL) {
	outb(channels[channel].ctrl, data);
	}
	else if (reg < 0x08) {
	outb(channels[channel].base + reg, data);
	}
}

u8 ide_cntrl_read_reg(u8 channel, u16 reg)
{
	if (reg == ATA_REG_ALTSTATUS) {
	return inb(channels[channel].ctrl);
	}
	else if (reg < 0x08) {
	return inb(channels[channel].base + reg);	
	}
else
	kprint("Invalid ATA register used");
	return 0;
}


void ide_read_ident_space_buffer(int ch)
{	
	 u32 count = 256;
	 u16 port = channels[ch].base;
	__asm__ __volatile__("rep insw" : : "c"(count), "d"(port), "D"(buffer)); /*buffer == edi*/
}

void ata_init_and_detect_drives(void)
{
	
	register_device(14, ata_interrupt_handler);
	register_device(15, ata_interrupt_handler);

    channels[ATA_PRIMARY].base  = ATA_REG_BASE_PRI;
    channels[ATA_PRIMARY].ctrl  = ATA_REG_DEV_CONTROL_PRI;
    channels[ATA_SECONDARY].base  = ATA_REG_BASE_SEC;
    channels[ATA_SECONDARY].ctrl  = ATA_REG_DEV_CONTROL_SEC;
	/* loop through every ATA device*/
	int count = 0;
	int i;
	int dev; 
	int k;
	u32 status, err = 0, type = IDE_ATA;
	for(i = 0; i < 2; i++)
		{
		for(dev = 0; dev < 2; dev++)
			{
			/* select the drive */
			ide_cntrl_write_reg(i, ATA_REG_HDDEVSEL, 0xa0 | (dev << 4));
			sleep(100);
			/* send identify command*/
			ide_cntrl_write_reg(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
			sleep(100);	
			/* Polling */
			if (ide_cntrl_read_reg(i, ATA_REG_STATUS) == 0) 
			{
				kprint("No device!");
				continue;	
			} 
			/*Probe for ATAPI Devices:*/
			if (err != 0) {
			u8 cl = ide_cntrl_read_reg(i, ATA_REG_LBA1);
			u8 ch = ide_cntrl_read_reg(i, ATA_REG_LBA2);
 
            if (cl == 0x14 && ch ==0xEB)
               type = IDE_ATAPI;
            else if (cl == 0x69 && ch == 0x96)
               type = IDE_ATAPI;
            else
               {			
				  continue;
			   }
            ide_cntrl_write_reg(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
			sleep(100);
         }				
			/*get identication space buffer*/
			ide_read_ident_space_buffer(i);
			ide_devices[count].Channel      = i;
			ide_devices[count].Drive        = dev;
			ide_devices[count].Capabilities = *((unsigned short *)(buffer + ATA_IDENT_CAPABILITIES));
			ide_devices[count].CommandSets  = *((u32 *)(buffer + ATA_IDENT_COMMANDSETS));
         if (ide_devices[count].CommandSets & (1 << 26))
            ide_devices[count].Size  = *((u32 *)(buffer + ATA_IDENT_MAX_LBA_EXT));
         else
            ide_devices[count].Size  = *((u32 *)(buffer + ATA_IDENT_MAX_LBA));

         for(k = 0; k < 40; k += 2) {
            ide_devices[count].Model[k] = buffer[ATA_IDENT_MODEL + k + 1];
            ide_devices[count].Model[k + 1] = buffer[ATA_IDENT_MODEL + k];}
            
			ide_devices[count].Model[40] = 0; 
		
			count++;
      }
	}	
   for (i = 0; i < 4; i++)
     {
         kprint(" Found %s Drive %dMB - %s Drive: %d Channel %d\n",
            (const char *[]){"ATA", "ATAPI"}[ide_devices[i].Type],         /* Type */
            ide_devices[i].Size / 1024 ,               /* Size */
            ide_devices[i].Model,
            ide_devices[i].Drive,
            ide_devices[i].Channel);		
	}
		ide_cntrl_write_reg(ATA_PRIMARY, ATA_REG_CONTROL, 0);
		ide_cntrl_write_reg(ATA_SECONDARY, ATA_REG_CONTROL, 0);
}

void ide_cntrl_access_sector(u8 NumSectors, u32 lba_addr, u8 direction, u8 drive,  u32 edi)
{
	 unsigned int  channel = ide_devices[drive].Channel;
	 while (ide_cntrl_read_reg(channel, ATA_REG_STATUS) & ATA_SR_BSY)
      ; 
	/*select drive*/
	ide_cntrl_write_reg(channel, ATA_REG_HDDEVSEL, 0xE0 | (ide_devices[drive].Drive << 4) | lba_addr & 0xF000000 >> 24);

	ide_cntrl_write_reg(channel, ATA_REG_SECCOUNT0, NumSectors);
	ide_cntrl_write_reg(channel, ATA_REG_LBA0, (lba_addr & 0x00000FF) >> 0);
	ide_cntrl_write_reg(channel, ATA_REG_LBA1, (lba_addr & 0x000FF00) >> 8);
	ide_cntrl_write_reg(channel, ATA_REG_LBA2, (lba_addr & 0x0FF0000) >> 16);
	
	/*send read or write command*/
	if(direction == 0)
		ide_cntrl_write_reg(channel, ATA_REG_COMMAND, ATA_CMD_READ_PIO);
	else
		ide_cntrl_write_reg(channel, ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
			
    unsigned int  bus = channels[channel].base;
    unsigned int  words      = 256; 
    
	/*read or write sector from disc*/
if(direction == 0) /*read*/
{ 
	ide_cntrl_write_reg(channel, ATA_REG_CONTROL, 0);
	int i;
      for (i = 0; i < NumSectors; i++) {
             kprint(".");
         __asm__ __volatile__("rep insw" : : "c"(words), "d"(bus), "D"(edi)); 
         edi += (words*2);
      } 
}
else /*write*/
{
	u16 *words = (u16 *)edi;
	u16 port = channels[channel].base;
	int i;
	for (i=0; i < 256 * NumSectors; i++) {
		outw(port, words[i]);
		}
	}
}
#define WRITE_SECTOR 1
#define READ_SECTOR 0
void read_disc_sector()
{
	/*testing*/
	u32 edi[1024];
	ide_cntrl_access_sector(1,0,READ_SECTOR,1,edi);
    int c;
	for ( c = 0; c < 512; c++ )
	{
	 kprint ("%d ", edi[ c ]);	 
	}
	/*error checking*/
}

void write_disc_sector()
{
	ide_cntrl_access_sector(1,0,WRITE_SECTOR,1,0x0100);	 
	/*error checking*/
}

u32 ata_interrupt_handler(u32 esp) {
	ata_irq_counts++;
	/*kprint("ATA irq count: %d", ata_irq_counts);*/
	return esp;
}

