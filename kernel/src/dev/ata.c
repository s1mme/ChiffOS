#include <types.h>
#include <video.h>
#include <kutils.h>
#include <irq.h>
#include <ata.h>


u16 buffer[256];
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
	u8 status, err = 0, type = IDE_ATA;
	for(i = 0; i < 2; i++)
		{
		for(dev = 0; dev < 2; dev++)
			{
			/* disable irq */
			ide_cntrl_write_reg(ATA_PRIMARY, ATA_REG_CONTROL, 2);
			ide_cntrl_write_reg(ATA_SECONDARY, ATA_REG_CONTROL, 2);

			/* select the drive */
			ide_cntrl_write_reg(i, ATA_REG_HDDEVSEL, 0xa0 | (dev << 4));
					
			/* send identify command*/
			ide_cntrl_write_reg(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
					
			/* Polling */
			if (ide_cntrl_read_reg(i, ATA_REG_STATUS) == 0) 
			{
				kprint("No device!");
				continue;	
			}
 
			while(1) {
				status = ide_cntrl_read_reg(i, ATA_REG_STATUS);
				if ((status & ATA_SR_ERR)) {err = 1; break;}
				if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break;
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
				   kprint("Unknown device type");
				   continue;
			   }
 
            ide_cntrl_write_reg(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
            
         }				
			/*get identication space buffer*/
			ide_read_ident_space_buffer(i);
			
			ide_devices[count].Type = type;
			
         if (ide_devices[count].CommandSets & (1 << 26))
            ide_devices[count].Size  = *((u32 *)(buffer + ATA_IDENT_MAX_LBA_EXT));
         else
            ide_devices[count].Size  = *((u32 *)(buffer + ATA_IDENT_MAX_LBA));

         for(k = 0; k < 40; k += 2) {
            ide_devices[count].Model[k] = buffer[ATA_IDENT_MODEL + k + 1];
            ide_devices[count].Model[k + 1] = buffer[ATA_IDENT_MODEL + k];}
            
			ide_devices[count].Model[40] = 0; /* Terminate String */
 
			count++;
      }
	}	
   /* Print Summary*/
   for (i = 0; i < 4; i++)
     {
         kprint("[INFO] Found %s Drive %dGB - %s\n",
			ide_devices[i].Type,        
            ide_devices[i].Size / 1024 / 1024 / 2,              
            ide_devices[i].Model);
		
	}
		ide_cntrl_write_reg(ATA_PRIMARY, ATA_REG_CONTROL, 0);
		ide_cntrl_write_reg(ATA_SECONDARY, ATA_REG_CONTROL, 0);
}

void ide_cntrl_access_sector(u32 NumSectors, u16 lba_addr, u8 direction, u8 drive, u8 channel)
{
	/*select drive*/
	ide_cntrl_write_reg(channel, ATA_REG_HDDEVSEL, 0xE0 | (ide_devices[drive].Drive << 4) | lba_addr & 0xF000000 >> 24);
	 
	ide_cntrl_write_reg(channel, ATA_REG_SECCOUNT1, NumSectors);
	ide_cntrl_write_reg(channel, ATA_REG_LBA0, (lba_addr & 0x00000FF) >> 0);
	ide_cntrl_write_reg(channel, ATA_REG_LBA1, (lba_addr & 0x000FF00) >> 8);
	ide_cntrl_write_reg(channel, ATA_REG_LBA2, (lba_addr & 0x0FF0000) >> 16);
	 
	/*read or write sector from disc*/ 
	 if(direction == 0) /*read*/
	 {
		u16 *words = (u16 *)buffer;
		u32 count = 256 * NumSectors; 
		u16 port = channels[channel].base;
		__asm__ __volatile__("rep insw" : : "c"(count), "d"(port), "D"(words)); 
	 }
	 else  /*write*/
	 {
		u16 *words = (u16 *)buffer;
		u16 port = channels[channel].base;
		int i;
		for (i=0; i < 256 * NumSectors; i++) {
		outw(port, words[i]);
		}
	 }
}

void read_disc_sector()
{
	/*error checking*/
}

void write_disc_sector()
{
	/*error checking*/
}


u32 ata_interrupt_handler(u32 esp) {

	ata_irq_counts++;
	return esp;
}

