#include <fat.h>
#include <video.h>
#include <types.h>
#include <heapmngr.h>
#include <kutils.h>
#include <ata.h>
BOOTSECTOR_t *bootsector;
MOUNT_INFO _MountInfo;
DIRECTORY *directory;
void read_file(FILE file);
FILE parse_dir( char* DirectoryName);
void mount_fat32()
{
	set_term_color(make_color(COLOR_BLACK,COLOR_WHITE));
	bootsector = (BOOTSECTOR_t*)kmalloc(sizeof(BOOTSECTOR_t));
	read_disc_sector(0,bootsector,0);
	
	_MountInfo.numSectors     = bootsector->total_sectors;
	_MountInfo.rootOffset     = (bootsector->num_fats * bootsector->sectors_per_fat) + bootsector->reserved_sectors;
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
	
	FILE file = parse_dir("MUU     TXT");
	ls_dir();
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

	    for (i=0; i<6; i++) {		
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
#define SECTOR_PER_ClUSTER 8
#define CLUSTER_SIZE 512*8
#define SECTOR_SIZE 512
#define FIRST_FAT_SECTOR 1
void read_file(FILE file)
{
u8 FAT_table[CLUSTER_SIZE];
u32 sector_count = file.fileLength/SECTOR_SIZE;
unsigned int cluster_start_lba = _MountInfo.rootOffset+(file.currentCluster - 2) * SECTOR_PER_ClUSTER;
read_disc_sector(sector_count,FAT_table,cluster_start_lba);

int i;
for(i = 0; i < file.fileLength; i++)
	{
	kputch(FAT_table[i]);
	}
}


void open_dir()
{
	
}
#define NUM_FILES 8
void ls_dir()
{

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
						kprint("%s %d BYTES\n",name, directory->FileSize);
    	 directory++;
	}
}
