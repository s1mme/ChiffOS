#include <fat.h>
#include <video.h>
#include <types.h>
#include <heapmngr.h>
#include <kutils.h>
#include <ata.h>
#include <elf.h>
BOOTSECTOR_t *bootsector;
MOUNT_INFO _MountInfo;
DIRECTORY *directory;
void read_file(FILE file);
void write_file(FILE file,char *buf, u8 method );
u8 *ls_dir();
FILE parse_dir( char* DirectoryName);

char file_meta_data[] =
{
 0x41, 0x6d, 0x0, 0x75, 0x00, 0x75, 0x00, 0x2e,  0x00, 0x74, 0x00, 0x0f, 0x00, 0xaf, 0x78, 0x00,  
 0x74, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
 0x4d, 0x55, 0x55, 0x20, 0x20, 0x20, 0x20, 0x20, 0x54, 0x58, 0x54, 0x20, 0x00, 0x00, 0x39, 0x0a,  
 0x45, 0x43, 0x45, 0x43, 0x00, 0x00, 0x39, 0x0a,  0x45, 0x43, 0x03, 0x00, 0x1b, 0x00, 0x00, 0x00,
 
 0x41, 0x6d, 0x0, 0x75, 0x00, 0x75, 0x00, 0x2e,  0x00, 0x74, 0x00, 0x0f, 0x00, 0xaf, 0x78, 0x00,  
 0x74, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
 0x4d, 0x55, 0x55, 0x20, 0x20, 0x20, 0x20, 0x20, 0x54, 0x58, 0x54, 0x20, 0x00, 0x00, 0x39, 0x0a,  
 0x45, 0x43, 0x45, 0x43, 0x00, 0x00, 0x39, 0x0a,  0x45, 0x43, 0x03, 0x00, 0x1b, 0x00, 0x00, 0x00,
};

void mount_fat32()
{
	set_term_color(make_color(COLOR_BLACK,COLOR_WHITE));
	bootsector = (BOOTSECTOR_t*)kmalloc(sizeof(BOOTSECTOR_t));
	read_disc_sector(0,bootsector,0);
	
	_MountInfo.numSectors = bootsector->total_sectors;
	_MountInfo.rootOffset = (bootsector->num_fats * bootsector->sectors_per_fat) + bootsector->reserved_sectors;
#ifdef DEBUGGING	
	kprint("Sectors available: %x\n", _MountInfo.numSectors );
	kprint("rootOffset: %d\n", _MountInfo.rootOffset);
	kprint("Test: %x\n", (u32)&bootsector->total_sectors - (u32)bootsector);
	kprint("root_cluster_num: %d\n", bootsector->root_cluster_num);
	kprint("num_direntries: %d\n",bootsector->num_direntries);
	kprint("sector per cluster %d\n", bootsector->sectors_per_cluster);
	kprint("sectors_per_fat %d\n", bootsector->num_fats);
#endif	
	free(bootsector);
}

void FAT_testing()
{	
	FILE file;
	file_meta_data[32] = 'W';

	/*write_file(file,file_meta_data,1);*/
	u8 *filename = ls_dir();
	file = parse_dir("TEST       ");
		
	/*write_file(file,"hello how are you?",2);*/
	read_file(file);	
}

#define FS_FILE       0
#define FS_DIRECTORY  1
#define FS_INVALID    2
FILE parse_dir( char* DirectoryName)
{
	FILE file;
	unsigned char buf[512];
	directory = (DIRECTORY*)kmalloc(sizeof(directory));
	
	read_disc_sector(0,buf,_MountInfo.rootOffset);

	int i;
	directory = (DIRECTORY*) buf;

	    for (i=0; i<128; i++) {		
    			char name[11];
    			memcpy (name, directory->Filename, 11);
    			name[11]=0;
						
				if (strncmp (DirectoryName, name,11) == 0) {
					#ifdef DEBUGGING
					kprint("Filename: %s\n", name);
					kprint("FileSize: %d BYTES\n", directory->FileSize);
					kprint("Attrib: %d\n", directory->Attrib);
					kprint("FirstCluster %d\n", directory->FirstCluster);
					#endif
					
					file.id             = 0;
					file.currentCluster = directory->FirstCluster;
					file.eof            = 0;
					file.fileLength     = directory->FileSize;
					
					if (directory->Attrib == 0x10)
					file.flags = FS_DIRECTORY;
					else
					file.flags = FS_FILE;
					return file;
					free(directory);
					
		}
    	 directory++;
	}
	return file;
}
#define SECTOR_PER_CLUSTER 8
#define CLUSTER_SIZE 512*25
#define SECTOR_SIZE 512
#define FIRST_FAT_SECTOR 1
u8 FAT_table[CLUSTER_SIZE];
void read_file(FILE file )
{

u32 sector_count = file.fileLength/SECTOR_SIZE;
u32 cluster_start_lba = _MountInfo.rootOffset+(file.currentCluster - 2) * SECTOR_PER_CLUSTER;
read_disc_sector(sector_count,FAT_table,cluster_start_lba);

if (!parse_elf(FAT_table, file.fileLength))
	kprint("\nCannot start program!\n");  
	               
/*
int i;
for(i = 0; i < file.fileLength; i++)
	{	
		if(!file.eof)
			kputch(FAT_table[i]);
	}*/
}

void write_file(FILE file , char *buf, u8 method)
{
u32 cluster_start_lba;
if(method == 1) /*write metadata e.g create files*/
cluster_start_lba = _MountInfo.rootOffset;

if(method == 2) /*write contents to that file*/
cluster_start_lba = _MountInfo.rootOffset +(file.currentCluster - 2) * SECTOR_PER_CLUSTER;

write_disc_sector(cluster_start_lba,buf);
}

#define NUM_FILES 8

u8 *ls_dir()
{
	FILE file;
	unsigned char buf[512];
	directory = (DIRECTORY*)kmalloc(sizeof(directory));	
	read_disc_sector(0,buf,_MountInfo.rootOffset);

	int i;
	directory = (DIRECTORY*) buf;

	    for (i=0; i<NUM_FILES; i++) {		
    			char name[11];
    			memcpy (name, directory->Filename, 11);
    			name[11]=0;
    				if (directory->Attrib == 0x20 || directory->Attrib == 0x10)
    				{
						kprint("%s %d BYTES\n",name, directory->FileSize);
						/*return directory->Filename;*/
					}
    	 directory++;    	 
	}
	/*SUBdirs and subfiles and prints out subfiles data*/
	directory = (DIRECTORY*) &FAT_table;

	    for (i=0; i<NUM_FILES; i++) {		
    			char name[11];
    			memcpy (name, directory->Filename, 11);
    			name[11]=0;
    				if (directory->Attrib == 0x20)
    				{
						kprint("%s %d BYTES\n",name, directory->FileSize);
						file.id             = 0;
						file.currentCluster = directory->FirstCluster;
						file.eof            = 0;
						file.fileLength     = directory->FileSize;
					    read_file(file);					    
					}
    	 directory++;
	}
}
