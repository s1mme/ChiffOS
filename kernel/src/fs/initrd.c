#include <initrd.h>
#include <heapmngr.h>
#include <types.h>
initrd_header_t *initrd_header;     
initrd_file_header_t *file_headers; 
DIR *initrd_root;             
DIR *initrd_dev;              
DIR *root_nodes;             
int nroot_nodes;                  

struct dirent dirent;

static u32 initrd_read(DIR *node, u32 offset, u32 size, u8 *buffer)
{
    initrd_file_header_t header = file_headers[node->inode];	 
    if (offset > header.length)
        return 0;
    if (offset+size > header.length)
        size = header.length-offset;
    memcpy(buffer, (u8*) (header.offset+offset), size);
    return size;
}

static struct dirent *initrd_readdir(DIR *node, u32 index)
{
    if (node == initrd_root && index == 0)
    {
      strcpy(dirent.name, "dev");
      dirent.name[3] = 0;
      dirent.ino = 0;
      return &dirent;
    }

    if (index-1 >= nroot_nodes)
        return 0;
    strcpy(dirent.name, root_nodes[index-1].name);
    dirent.name[strlen(root_nodes[index-1].name)] = 0;
    dirent.ino = root_nodes[index-1].inode;
    return &dirent;
}

static DIR *initrd_finddir(DIR *node, char *name)
{
    if (node == initrd_root &&
        !strcmp(name, "dev") )
        return initrd_dev;

    int i;
    for (i = 0; i < nroot_nodes; i++)
        if (!strcmp(name, root_nodes[i].name))
            return &root_nodes[i];
    return 0;
}

DIR *install_initrd(u32 location)
{
   
    initrd_header = (initrd_header_t *)location;
    file_headers = (initrd_file_header_t *) (location+sizeof(initrd_header_t));	



    initrd_root = (DIR*)malloc_(sizeof(DIR));
	strcpy(initrd_root->name, "initrd");
	initrd_root->flags = FS_DIRECTORY;
  
    initrd_root->readdir = &initrd_readdir;		
    initrd_root->finddir = &initrd_finddir;
    

    initrd_dev = (DIR*)malloc_(sizeof(DIR));
    strcpy(initrd_dev->name, "dev");
    initrd_dev->flags = FS_DIRECTORY;
   
    initrd_dev->readdir = &initrd_readdir;
    initrd_dev->finddir = &initrd_finddir;
    

    root_nodes = (DIR*)malloc_(sizeof(DIR) * initrd_header->nfiles); 
    nroot_nodes = initrd_header->nfiles;

	int i;

   for (i = 0; i < initrd_header->nfiles; i++)
   {
       file_headers[i].offset += location;
      
       strcpy(root_nodes[i].name, &file_headers[i].name);
       root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
       root_nodes[i].length = file_headers[i].length;
       root_nodes[i].inode = i;
       root_nodes[i].flags = FS_FILE;
       root_nodes[i].read = &initrd_read;
       root_nodes[i].write = 0;
       root_nodes[i].readdir = 0;
       root_nodes[i].finddir = 0;
       root_nodes[i].open = 0;
       root_nodes[i].close = 0;
       root_nodes[i].impl = 0;
   }
   return initrd_root;
} 


