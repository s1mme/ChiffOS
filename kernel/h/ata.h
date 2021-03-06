#ifndef ATA_H
#define ATA_H
#include <types.h>

typedef struct IDEChannelRegisters {
   u16 base;  
   u16 ctrl;  
   u16 bmide; 
   u8  nIEN;  
};
typedef struct IDEChannelRegisters CHANNELS;

typedef struct ide_device {
   u8  Reserved;   
   u8  Channel;     
   u8  Drive;     
   u16 Type;       
   u16 Signature;   
   u16 Capabilities;
   u32   CommandSets;
   u32   Size;        
   u8  Model[41];   
};

typedef struct ide_device IDE_DEVICES;
void ata_init_and_detect_drives();
u16 read_disc_sector(u32 sector, u32 *edi, u32 LBAnum);
void write_disc_sector(u32 lba_start_num,char buf[512]);

#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_REG_DATA       0x00
#define ATA_PRIMARY 0x00
#define      ATA_SECONDARY    0x01
 
#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24

#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34

#define      ATA_READ      0x00
#define      ATA_WRITE     0x01

#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_CMD_IDENTIFY   0xEC
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_REG_SECCOUNT1  0x08

#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B

#define ATA_REG_BASE_PRI 0x1f0
#define ATA_REG_BASE_SEC 0x170
#define ATA_REG_DEV_CONTROL_PRI 0x3f6 
#define ATA_REG_DEV_CONTROL_SEC 0x376

#define ATA_CMD_READ_PIO          0x20

#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

#define IDE_ATA        0x00
#define IDE_ATAPI      0x01
 
#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01


#define ATA_ER_BBK      0x80
#define ATA_ER_UNC      0x40
#define ATA_ER_MC       0x20
#define ATA_ER_IDNF     0x10
#define ATA_ER_MCR      0x08
#define ATA_ER_ABRT     0x04
#define ATA_ER_TK0NF    0x02
#define ATA_ER_AMNF     0x01

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200


/* ATA commands we use */
#define ATA_CMD_IDENTIFY 0xec
#define ATA_CMD_READ_SECTORS 0x20
#define ATA_CMD_WRITE_SECTORS 0x30
#define ATA_CMD_READ_MULTIPLE 0xc4
#define ATA_CMD_WRITE_MULTIPLE 0xc5
#define ATA_CMD_SET_MULTIPLE_MODE 0xc6
#define ATA_CMD_SET_FEATURES 0xef

#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D
#endif
