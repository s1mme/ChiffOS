#include <fat.h>
#include <video.h>
#include <types.h>
#include <heapmngr.h>
#include <kutils.h>
#include <ata.h>
#include <elf.h>
#include <proc.h>
#include <fat.h>
BOOTSECTOR_t *bootsector;
MOUNT_INFO _MountInfo;
DIRECTORY *directory;

int read_file(FILE file);


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
	int file = open("MUU     TXT",0); /* todo: fix dos names */
	
	int sz = read(file,0,current_task->fdtable[file].size);
	
	kprint("size: %d", sz);
	
	u8 *text = "HELLO!";
	file = open("MUU     TXT",1);
	kprint("file number %d", file);
	write(file, text, strlen(text));
	
	file = open("MUU     TXT",0);
	kprint("\n");
	sz = read(file,0,current_task->fdtable[file].size);
	kprint("\n");
	
	/*
	file3 = parse_dir("TEST       ");
		
	write_file(file3,"goat",2);
	/*read_elf(file);	*/
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
#define CLUSTER_SIZE 512*90
#define SECTOR_SIZE 512
#define FIRST_FAT_SECTOR 1
u8 FAT_table[CLUSTER_SIZE];
void read_elf(FILE file )
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


#define O_RDONLY 0

int open(const char *path, int mode) {
	FILE node = parse_dir(path);
	int fd =4;
	fd++;
	if (mode != O_RDONLY)
	{
	FILE file;	
	
		write_file(node,file_meta_data,1); /* todo: specify filename with path*/
		
		current_task->fdtable[fd].node[6] = node; /* node[x] -> x = size of write message , todo: fix this !*/
		return fd;		
	}
	else
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
