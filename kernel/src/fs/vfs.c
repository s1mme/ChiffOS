#include <types.h>
#include <initrd.h>
#include <vfs.h>
#include <kernel.h>

int read_fs(DIR *node, int offset, int size, u8 *buffer)
{
    if (node->read != 0)
        return node->read(node, offset, size, buffer);
    else
        return 0;
}


struct dirent *readdir_fs(DIR *node, int index)
{
    if ( (node->flags&0x7) == FS_DIRECTORY &&
         node->readdir != 0 )
        return node->readdir(node, index);
    else
        return 0;
}

DIR *finddir_fs(DIR *node, char *name)
{
    if ( (node->flags&0x7) == FS_DIRECTORY &&
         node->finddir != 0 )
        return node->finddir(node, name);
    else
        return 0;
}



uint32_t write_fs(DIR* node, uint32_t offset, uint32_t size, u8* buffer)
{
    if (node->write != 0) 
        return node->write(node, offset, size, buffer);
    else
        return 0;
}

void open_fs( u8 read, u8 write)
{
	//DIR* node = fs_root;
  //  if (node->open != 0) 
    //    return node->open(node);
}

void close_fs(DIR* node)
{
    if (node->close != 0) 
        return node->close(node);
}


int ioctl_fs(DIR *node, int request, void * argp) {
if (node->ioctl) {
return node->ioctl(node, request, argp);
} else {
return -1;
}
}
