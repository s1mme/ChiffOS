#ifndef INITRD_H
#define INITRD_H

#include <types.h>
#include <vfs.h>
typedef struct
{
    u32 nfiles; // The number of files in the ramdisk.
} initrd_header_t;

typedef struct
{
    u8 magic;     
    s8 name[64];  // Filename.
    u32 offset;   
    u32 length;   
} initrd_file_header_t;


DIR *install_initrd(u32 location);

#endif
