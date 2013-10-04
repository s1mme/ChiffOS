#include <fat.h>
#include <video.h>
#include <types.h>
#include <heapmngr.h>
#include <kutils.h>
BOOTSECTOR_t *bootsector;
MOUNT_INFO _MountInfo;
DIRECTORY *directory;

FILE parse_dir(const char* DirectoryName);
void mount_fat32()
{
	bootsector = (BOOTSECTOR_t*)kmalloc(sizeof(BOOTSECTOR_t));
	read_disc_sector(0,bootsector,0);
	
	_MountInfo.numSectors     = bootsector->total_sectors;
	_MountInfo.rootOffset     = (bootsector->num_fats * bootsector->sectors_per_fat) + bootsector->reserved_sectors;
	kprint("Sectors available: %x\n", _MountInfo.numSectors );
	kprint("rootOffset: %d\n", _MountInfo.rootOffset);
	kprint("Test: %x\n", (u32)&bootsector->total_sectors - (u32)bootsector);
	kprint("root_cluster_num: %d\n", bootsector->root_cluster_num);
	kprint("num_direntries: %d\n",bootsector->num_direntries);
	kprint("sector per cluster %d\n", bootsector->sectors_per_cluster);
	free(bootsector);
	
	FILE file = parse_dir("muu.txt");
	read_file(file);
}


FILE parse_dir(const char* DirectoryName)
{
	FILE file;
	unsigned char buf[512];
	directory = (DIRECTORY*)kmalloc(sizeof(directory));
	read_disc_sector(0,buf,_MountInfo.rootOffset);
	
	const char *DosFileName;
	ToDosFileName (DirectoryName, DosFileName, 11);
	int i;
	directory = (DIRECTORY*) buf;
	    for (i=0; i<128; i++) {
		const char *DosFileName;
		ToDosFileName (DirectoryName, DosFileName, 11);
		
    			char name[9];
    			memcpy (name, directory->Filename, 11);
    			name[11]=0;
    		
				if (strcmp (DosFileName, name) == 0) {
					kprint("Filename: %s\n", name);
					kprint("FileSize: %d BYTES\n", directory->FileSize);
					kprint("Attrib: %d\n", directory->Attrib);
					kprint("FirstCluster %d\n", directory->FirstCluster);
					
					file.id             = 0;
					file.currentCluster = directory->FirstCluster;
					file.eof            = 0;
					file.fileLength     = directory->FileSize;
					return file;
					break;
		}
    	 directory++;
	}
	
}
#define CLUSTER_SIZE 512*8
#define FIRST_FAT_SECTOR 1
void read_file(FILE file)
{
	
u8 FAT_table[CLUSTER_SIZE];
u32 fat_offset = file.currentCluster * 4;
u32 fat_sector = FIRST_FAT_SECTOR + (fat_offset / CLUSTER_SIZE);
u32 ent_offset = fat_offset % CLUSTER_SIZE;


 kprint("fat_sector %d\n", fat_sector);
 read_disc_sector(0,FAT_table,fat_sector);
 kprint("FAT_table %x\n", FAT_table[0]);
 unsigned int table_value = *(unsigned int*)&FAT_table[ent_offset] & 0x0FFFFFFF;
 kprint("table_value %d\n", table_value);
 kprint("file.currentCluster %d", file.currentCluster);

}
