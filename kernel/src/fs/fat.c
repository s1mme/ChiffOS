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
u32 first_free_cluster();

elf_header_t * read_elf(FILE file);


FILE ls_dir( char* DirectoryName, u32 offset);
FILE parse_dir( char* DirectoryName);

char file_meta_data[] =
{
 0x41, 0x6d, 0x0, 0x75, 0x00, 0x75, 0x00, 0x2e,  0x00, 0x74, 0x00, 0x0f, 0x00, 0xaf, 0x78, 0x00,  
 0x74, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
 0x4d, 0x55, 0x55, 0x20, 0x20, 0x20, 0x20, 0x20, 0x54, 0x58, 0x54, 0x20, 0x00, 0x00, 0x39, 0x0a,  
 0x45, 0x43, 0x45, 0x43, 0x00, 0x00, 0x39, 0x0a,  0x45, 0x43, 0x03, 0x00, 0x1b, 0x00, 0x00, 0x00

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
	elf = ls_dir("test",0);
	elf_program = read_elf(elf);
	create_process((void*)elf_program->entry,THREAD,0,0,0);
}


void FAT_vesa()
{
	elf_header_t * elf_program;
	int file = 0;
	int sz = 0;
	FILE elf;

	
	elf = ls_dir("VESA",0);
	
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
	
	elf = ls_dir("GLOADER",0);
	
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
	FILE test; 
	
	/*test = ls_dir("vesa",0);
	file = open("znark.txt",0);
	/*read(file,0,current_task->fdtable[file].size);*/
		
	char *name = "epzordiu";
	next = open(name,1);		
	write(next,"HEY how are you doin?!", strlen(name)); 
	next = open("epzordiu.TXT",0);	
    read(next,0,current_task->fdtable[next].size);
}

#define FS_FILE       0
#define FS_DIRECTORY  1
#define FS_INVALID    2
#define ATTRIB_READONLY 0x1
#define ATTRIB_HIDDEN 0x2
#define ATTRIB_SYSTEM 0x4
#define ATTRIB_VOLUME_ID 0x8
#define ATTRIB_DIR 0x10
#define ATTRIB_ARCHIVE 0x20

/* Used for Long File Name entries */
#define ATTRIB_LFN (ATTRIB_READONLY | ATTRIB_HIDDEN | ATTRIB_SYSTEM | ATTRIB_VOLUME_ID)

#define SECTOR_PER_CLUSTER 8
#define CLUSTER_SIZE 512*60
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

int read_(FILE file )
{
	u32 sector_count = 1;/*file.fileLength/SECTOR_SIZE;*/

	u32 cluster_start_lba = _MountInfo.rootOffset+(3 - 2) * SECTOR_PER_CLUSTER;
	read_disc_sector(sector_count,FAT_table,cluster_start_lba);
}

int read_file(FILE file )
{
	/*kprint("fileLength: %d",file.fileLength);*/
	u32 sector_count = 1 ; /*file.fileLength/SECTOR_SIZE;*/
	u32 cluster_start_lba = _MountInfo.rootOffset+(file.currentCluster - 2) * SECTOR_PER_CLUSTER;
	read_disc_sector(sector_count,FAT_table,cluster_start_lba);
		kprint("\n");
	int i;
	for(i = 0; i < file.fileLength; i++)
	{	
		if(!file.eof)
			kputch(FAT_table[i]);
		
	}
	kprint("\n");
	return file.fileLength;
}
#define LBAoffset 5
void write_file(FILE file , char *buf, u8 method, u32 offset)
{	
	u32 cluster_start_lba;
	if(method == 1) /*write metadata e.g create files*/
	{
		cluster_start_lba = _MountInfo.rootOffset+1;	
		read_file(file);
		memcpy(FAT_table+offset, buf, 32);
		FAT_table[cluster_start_lba] = 0x0FFFFFFF;
		write_disc_sector(cluster_start_lba,FAT_table);
	}	

	if(method == 2) /*write contents to that file*/
	{
		cluster_start_lba =  _MountInfo.rootOffset+(3 - 2) * SECTOR_PER_CLUSTER;
		read_file(file);
		memcpy(FAT_table+offset, buf, 32);
		FAT_table[cluster_start_lba] = 0x0FFFFFFF;
		write_disc_sector(cluster_start_lba,FAT_table);
	}	
}


#define NUM_FILES 8

FILE ls_dir( char* DirectoryName, u32 offset)
{	
	DIRECTORY *lsdirectory;
	FILE file;
	char DosFileName[11];
	ToDosFileName (DirectoryName, DosFileName, 11);
	DosFileName[11]=0;
	unsigned char buf[512];
	lsdirectory = (DIRECTORY*)kmalloc(32);	
	u32 backupinfo = _MountInfo.rootOffset;
	memset(lsdirectory, 0, 32);
	read_disc_sector(0,buf,_MountInfo.rootOffset+offset);
	
int counter = 0;
	int i;
	while(counter < 128)
	{
	lsdirectory = (DIRECTORY*) buf;

	    for (i=0; i<NUM_FILES; i++) {		
    			char name[11];
    			memcpy (name, lsdirectory->Filename, 11);
    			name[11]=0;
    				if (lsdirectory->Attrib == 0x20 || lsdirectory->Attrib == 0x10)
    				{
					if (strncmp (DosFileName, lsdirectory->Filename,11) == 0) {
						kprint("Filename: %s\n", name);
					kprint("FileSize: %d BYTES\n", lsdirectory->FileSize);
					kprint("Attrib: %d\n", lsdirectory->Attrib);
					kprint("FirstCluster %d\n", lsdirectory->FirstCluster);
					
					file.Attrib  		= lsdirectory->Attrib;
					memcpy (file.name, lsdirectory->Filename, 8);
					memcpy (file.Ext, lsdirectory->Ext, 3);
					file.id             = 0;
					file.currentCluster = lsdirectory->FirstCluster;
					file.eof            = 0;
					file.fileLength     = lsdirectory->FileSize;
					
					if (lsdirectory->Attrib == 0x10)
					file.flags = FS_DIRECTORY;
					else
					file.flags = FS_FILE;
					_MountInfo.rootOffset = backupinfo;
					/*free(directory);*/
					return file;
						/*return directory->Filename;*/
					}
				}
    	 lsdirectory++; 
    	counter++;
   	 
	}
	if(counter > 128)
		read_disc_sector(0,buf,_MountInfo.rootOffset+offset);
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
_MountInfo.rootOffset = backupinfo;
	return file;
}

u32 first_free_cluster() { 
	int i;
	for ( i=0; i <  _MountInfo.numSectors; i++) {
		if (FAT_table[i] == 0) return i;
	}
}

#define O_RDONLY 0
int fd =4;
int open(const char *path, int mode) {
	fd++;
	DIRECTORY *mdirectory;
	FILE node;
	node = ls_dir(path,0);
	
		 mdirectory = (DIRECTORY*)kmalloc(32);
		 memset(mdirectory, 0, 32);	
	if (mode == 1)
	{	
		FILE enode;
		memcpy (mdirectory->Filename, path, 8);
		memcpy (mdirectory->Ext, "TXT", 3);
		kprint("mdirectory->Filename %s",mdirectory->Filename );
		mdirectory->Attrib = 0x20;
		mdirectory->Reserved = 0x00;
		mdirectory->TimeCreatedMs = 0x00;
		mdirectory->TimeCreated = 0x0a39;
		mdirectory->DateCreated = 0x4345;
		mdirectory->DateLastAccessed = 0x4345; 
		mdirectory->LastModTime = 0x0000;
		mdirectory->LastModDate = 0x0a39;


		mdirectory->FirstCluster = 3;

		mdirectory->FileSize = 0x0000001b;  
		enode.currentCluster = 1; 
		enode.fileLength = 0x0000001b;
	
		char buffer[32];

		memcpy(buffer, mdirectory, 32); 
		write_file(enode,buffer,1,0); 
		
		current_task->fdtable[fd].node[5] = enode; /* node[x] -> x = size of write message , todo: fix this !*/
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
		write_file(current_task->fdtable[file].node[length],buf,2,0);
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
