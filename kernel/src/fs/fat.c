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

#define NUM_FILES 8
#define NUM_entries 16
BOOTSECTOR_t *bootsector;
MOUNT_INFO _MountInfo;
DIRECTORY *directory;
DIRECTORY *lsdirectory;

elf_header_t * read_elf(FILE file);


u8 *ls_dir();
FILE parse_dir( char* DirectoryName);

char file_meta_data[] =
{

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
	
	kprint("Sectors available: %x\n", _MountInfo.numSectors );
	kprint("rootOffset: %d\n", _MountInfo.rootOffset);
	kprint("Test: %x\n", (u32)&bootsector->total_sectors - (u32)bootsector);
	kprint("root_cluster_num: %d\n", bootsector->root_cluster_num);
	kprint("num_direntries: %d\n",bootsector->num_direntries);
	kprint("sector per cluster %d\n", bootsector->sectors_per_cluster);
	kprint("sectors_per_fat %d\n", bootsector->num_fats);

	free(bootsector);
}
u16 *surface;
VESA_MODE_INFO mib;
#define VESA_MODE 279
void FAT_testing_newlib()
{
	FILE elf;
	elf_header_t * elf_program;
	elf = parse_dir("test");
	elf_program = read_elf(elf);
	create_process((void*)elf_program->entry,THREAD,0,0,0);
}


void FAT_vesa()
{
	elf_header_t * elf_program;
	int file = 0;
	int sz = 0;
	FILE elf;

	
	elf = parse_dir("VESA");
	
	elf_program = read_elf(elf);
	
	*(u16*)0x3600 = VESA_MODE;
	
	create_process((void*)elf_program->entry,VM86,3,0,0);
	sleep(4000);
	memcpy(&mib, (void*)0x3600, sizeof(VESA_MODE_INFO));
    
	*(u16*)0x3600 = VESA_MODE | 0x400;
	
	
    kprint("PhysBasePtr: %x\n",mib.PhysBasePtr);
	
	kprint("XResolution: %d pixels \n",mib.XResolution);
	kprint("XResolution: %d pixels \n",mib.YResolution);
	kprint("BitsPerPixel: %d\n", mib.BitsPerPixel);
	kprint("WinSize %d\n", mib.WinSize);
	
	elf = parse_dir("GLOADER");
	
	elf_program = read_elf(elf);
	sleep(4000);
	surface = paging_getVirtaddr(mib.PhysBasePtr, 0x400);
	
	create_process((void*)elf_program->entry,VM86,3,0,0);
	sleep(500);
	
	memset(surface,0, mib.XResolution*mib.YResolution*2);	/* put whole screen painted! */
	
	memset(surface,50, mib.XResolution*mib.YResolution*2);

}
void FAT_file_testing()
{
	int file;					
	int next;

	file = open("gloader",0);
    read(file,0,current_task->fdtable[file].size);


	next = open("test.txt",0);
	read(next,0,current_task->fdtable[next].size);
	file = open("test.txt",1);
	write(file,"i am tired!", 5);	/* this overites first 16 entries */
	ls_dir();				
}

#define FS_FILE       0
#define FS_DIRECTORY  1
#define FS_INVALID    2
FILE parse_dir( char* DirectoryName)
{
	
	char DosFileName[11];
	ToDosFileName (DirectoryName, DosFileName, 11);
	DosFileName[11]=0;
	kprint("%s",  directory->Filename);
	FILE file;
	unsigned char buf[512];
	directory = (DIRECTORY*)kmalloc(sizeof(directory));
	
	read_disc_sector(0,buf,_MountInfo.rootOffset);

	int i;
	directory = (DIRECTORY*) buf;
	int counter = 0;
	while(counter < 64)
	{
		
	    for (i=0; i<128; i++) {		
    			char name[11];
    			memcpy (name, DosFileName, 11);
    			name[11]=0;
	
				if (strncmp (DosFileName, directory->Filename,11) == 0) {
					kprint("..");
					
					kprint("Filename: %s\n", name);
					kprint("FileSize: %d BYTES\n", directory->FileSize);
					kprint("Attrib: %d\n", directory->Attrib);
					kprint("FirstCluster %d\n", directory->FirstCluster);
					
					
					file.id             = 0;
					file.currentCluster = directory->FirstCluster;
					file.eof            = 0;
					file.fileLength     = directory->FileSize;
					
					if (directory->Attrib == 0x10)
					file.flags = FS_DIRECTORY;
					else
					file.flags = FS_FILE;
					/*free(directory);*/
					return file;
					
					
		}
    	 directory++;
    	 counter++;
	}
	if(counter > NUM_entries)
		read_disc_sector(0,buf,_MountInfo.rootOffset++);
  }
	return file;
}

#define SECTOR_PER_CLUSTER 8
#define CLUSTER_SIZE 512*70
#define SECTOR_SIZE 512
#define FIRST_FAT_SECTOR 1
u8 FAT_table[CLUSTER_SIZE];

elf_header_t * elf_header_;

elf_header_t * read_elf(FILE file )
{
	u32 sector_count = file.fileLength/SECTOR_SIZE;
	u32 cluster_start_lba = _MountInfo.rootOffset+(file.currentCluster - 2) * SECTOR_PER_CLUSTER;
	read_disc_sector(sector_count,FAT_table,cluster_start_lba);

	elf_header_ = parse_elf(FAT_table, file.fileLength);

	return elf_header_;   
/*
int i;
for(i = 0; i < file.fileLength; i++)
	{	
		if(!file.eof)
			kputch(FAT_table[i]);
	}*/
}

int read_file(FILE file )
{
	u32 sector_count = file.fileLength/SECTOR_SIZE;
	u32 cluster_start_lba = _MountInfo.rootOffset+(file.currentCluster - 2) * SECTOR_PER_CLUSTER;
	read_disc_sector(sector_count,FAT_table,cluster_start_lba);
	
	int i;
	for(i = 0; i < file.fileLength; i++)
	{	
		if(!file.eof)
			kputch(FAT_table[i]);
	}
	return file.fileLength;
}
#define LBAoffset 5
void write_file(FILE file , char *buf, u8 method)
{
	u32 cluster_start_lba;
	if(method == 1) /*write metadata e.g create files*/
		cluster_start_lba = _MountInfo.rootOffset;		/*if LBAoffset is 1 then jump 16 entries from root and put metadata there */

	if(method == 2) /*write contents to that file*/
	cluster_start_lba = _MountInfo.rootOffset +(file.currentCluster - 2) * SECTOR_PER_CLUSTER;

	write_disc_sector(cluster_start_lba,buf);
}


#define NUM_FILES 8

u8 *ls_dir()
{
	FILE file;
	unsigned char buf[512];
	lsdirectory = (DIRECTORY*)kmalloc(sizeof(lsdirectory));	
	read_disc_sector(0,buf,_MountInfo.rootOffset);

int counter = 0;
	int i;
	while(counter < 64)
	{
	lsdirectory = (DIRECTORY*) buf;

	    for (i=0; i<NUM_FILES; i++) {		
    			char name[11];
    			memcpy (name, lsdirectory->Filename, 11);
    			name[11]=0;
    				if (lsdirectory->Attrib == 0x20 || directory->Attrib == 0x10)
    				{
						if(counter > 32)
							{		
								kprint("%s %d BYTES cluster %d \n",name, lsdirectory->FileSize,lsdirectory->FirstCluster);				
							}
						if(counter < 8)
						kprint("%s %d BYTES cluster %d \n",name, lsdirectory->FileSize,lsdirectory->FirstCluster);
						/*return directory->Filename;*/
					}
    	 lsdirectory++; 
    	counter++;
   	 
	}
	if(counter > NUM_entries)
		read_disc_sector(0,buf,_MountInfo.rootOffset++);
  }
	
	/*SUBdirs and subfiles and prints out subfiles data*/
/*	directory = (DIRECTORY*) &FAT_table;

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
	}*/
	return lsdirectory->Filename;
}

#define O_RDONLY 0
int fd =4;
int open(const char *path, int mode) {
	FILE node = parse_dir(path);
	
	
	if (mode == 1)
	{
		fd++;
		/*file_meta_data[33] = path[0];*/
	/*	file_meta_data[32] = path[0];
		file_meta_data[33] = path[1];
		file_meta_data[34] = path[2];
		file_meta_data[35] = path[3];
		file_meta_data[36] = path[4];
		file_meta_data[37] = path[5];
		file_meta_data[38] = path[6];

		*/
		write_file(node,file_meta_data,1); /* todo: specify filename with path*/
		
		current_task->fdtable[fd].node[5] = node; /* node[x] -> x = size of write message , todo: fix this !*/
		return fd;		
	}
	if(mode == 0)
	{	
		current_task->fdtable[fd].node[node.fileLength] = node;
		current_task->fdtable[fd].size = node.fileLength;
		return fd;
		/* read file */
	}
}


int read(int file,  u8 *buffer, u32 size)
{
	
	if(file > 3)
	{
		read_file(current_task->fdtable[file].node[size]);	
	}

	/*else 
	stdio_read(current_task->fdtable[file].node,buffer,size);*/
	return current_task->fdtable[file].size;
}

int write(int file, char* buf, int length)
{
	int ret = 0;
	if(file > 3)
	{
		write_file(current_task->fdtable[file].node[length],buf,2);
	}
	else
	{
		const char *p = (const char *)buf;
		int i;
		for ( i = 0; i < length && *p; i++) {
			kputch(*p++);
	}
	ret++;
	}
return ret;
}
