#ifndef VFS_H
#define VFS_H



#define FS_FILE        0x01
#define FS_DIRECTORY   0x02


struct fs_node;
//function pointers
typedef int (*read_type_t)(struct fs_node*,int,int,unsigned char*);
typedef int        (*write_type_t)   (struct fs_node*,int,int,unsigned char*);
typedef int            (*open_type_t)    (struct fs_node*);
typedef void            (*close_type_t)   (struct fs_node*);
typedef struct dirent * (*readdir_type_t)(struct fs_node*,int);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*,char *name);
typedef int (*ioctl_type_t) (struct fs_node *, int request, void * argp);
typedef struct fs_node
{
   char name[128];     
   int mask;        
   int uid;        
   int gid;         
   int flags;      
   int inode;       
   int length;      
   int impl;        
  
   read_type_t read;
   write_type_t write;
   open_type_t open;
   close_type_t close;
   readdir_type_t readdir;
   finddir_type_t finddir;
   ioctl_type_t ioctl;

} DIR;

struct dirent
{
	 char d_name[128];
    char name[128]; 
    int ino;     
};


int read_fs(DIR *node, int offset, int size, unsigned char *buffer);
void open_fs(unsigned char read, unsigned char write);
struct dirent *readdir_fs(DIR *node, int index);
DIR *finddir_fs(DIR *node, char *name);

#endif
