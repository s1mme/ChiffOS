#include <types.h>
#include <irq.h>
#include <video.h>
#include <vmmngr.h>
#include <elf.h>
#include <fat.h>
#include <proc.h>
#include <syscalls.h>
#include <kutils.h>
#include <kbd.h>
#include <vesa.h>
void syscall_handler(struct regs *r);
#define BRK 6000000
int close(int file) {
return 1;
}

void *sbrk(int incr) {

static u32 brk = 0x1400000+BRK;
int prev_brk = brk;

if (incr == 0) {
	return (void *)prev_brk;
}

if (incr > 0) {
u32 new_end = brk + incr;
//if (!IS_PAGE_ALIGNED(new_end)) {
new_end &= 0xfffff000;
new_end += 0x1000;
//}*/
int i;

for (i = 0x1400000; i < new_end; i += 0x1000) 
_vmm_get_page_addr(i, 1, pd);

for (i = 0x1400000; i <new_end; i += 0x1000)	
_vmmngr_alloc_frame(_vmm_get_page_addr(i, 1, pd),0,1);

brk = new_end;

return (void *)prev_brk;
}
else {
printk("sbrk with negative argument: TODO"); // TODO
return 0;
	}
}

    
void restart();

#define EBADF            9

int fstat(int fd, struct stat *st) {
	//printk ( "SYSCALL : fstat(%d, %x)\n", fd, st);
    if (!st)return -1;
    memset(st, 0, sizeof(struct stat));
    //st->st_dev = 0xffff; // TODO
    //st->st_ino = current_task->fdtable[fd].node->inode; 
    //st->st_mode = 0666 | S_IFCHR; //current_task->fdtable[fd].node->mask;
    //st->st_size = current_task->fdtable[fd].node->length;
    //st->st_blksize = 128;   
    return 0;
}
 /*
int errno;
int isatty(int file) {
puts("isatty");
if (file < 0) {
	
errno = EBADF;
return 0; // [sic]
}

struct stat st;
int ret;
if ((ret = fstat(file, &st)) == 0)
return (S_ISCHR(st.st_mode)) ? 1 : 0;
else {
errno = -ret;
return 0; // [sic]
}
}
*/
int lseek(int fd, int offset, int whence) {
/*union {
	  
off_t o64;
u32 u32[2];
} arg, ret;
arg.o64 = offset;
//__asm__ __volatile__("int $0x80" : "=a" (ret.u32[0]), "=d" (ret.u32[1]) : "0" (9), "b" (fd), "c" (arg.u32[0]), "d"(arg.u32[1]), "S"(whence));
return ret.o64;*/
	return 1;
}

static int access(const char * file, int flags) {
	
//validate((void *)file);
//debug_print(INFO, "access(%s, 0x%x) from pid=%d", file, flags, getpid());
//DIR * node = open((char *)file, 0);
//if (!node) return -1;
//close_fs(node);
//return 0;
	return -1;
}

static int ioctl(int fd, int request, void * argp) {
			 return 1;
	if (fd >= current_task->fdtable[fd].node || fd < 0) {
return -1;
}

if (current_task->fdtable[fd].node == NULL) {
return -1;
}

//validate(argp);
DIR * node = current_task->fdtable[fd].node;
return ioctl_fs(node, request, argp);

}

void __cli()
{
__asm__ __volatile__ ("cli");	
}

int sti()
{
__asm__ __volatile__ ("sti");	
}

int __sseek64()
{
	return 0;
}

int __swrite64()
{
	return 0;
}


/*
#define EACCES 13
#define O_RDONLY 0
DIR* fsnode;
int open(const char *path, int mode) {

 if (mode != O_RDONLY)
  return -EACCES;

 int fd = 0;
 struct dirent* node = 0;
	
	 for (size_t i = 0; (node = readdir_fs(fs_root, i)) != 0; ++i)
    {
        DIR* fsnode = finddir_fs(fs_root, node->name);

        if ((fsnode->flags & 0x7) == FS_DIRECTORY)
        {
          
        }
        else
        {
           // printk("Size: %d bytes\t Name: %s\n", fsnode->length, node->name);

            if (strcmp(node->name, path) == 0)
            {
				
				
       
			 //printk("test1");
			 
			 current_task->fdtable[fd].node = fsnode;
			
    return fd;
    

}
	}
}
       
}
/*

int offset = 0;
int read(int file,  u8 *buffer, u32 size)
{
	if(size > 20)
	{		
		offset += 1;
		
		if (offset > size )
		{
		offset = 0;
		return -EBADF;
		}
	u32 sz = read_fs(current_task->fdtable[file].node, 0,size, buffer);
	return sz;	
}
else 
	stdio_read(current_task->fdtable[file].node,buffer,size);
}
*/



setxy(int x, int y)
{
	c_x = x;
	c_y = y;
}
setx(int x)
{
	c_x = x;
}
void nop()
{
__asm__ __volatile__ ("nop");	
}

void restart()
{
outb (0x64, 0xFE);
}


 void* syscalls[] =
{
&_exit,			//0
&puts,
&__cli,
&getchar,
&putch,
&open, 			//5
&read,
&execve,
&setxy,
&clear,
&restart, 		//10
&setx,
&write,
&put_dec,
&fstat,
&nop, 			//15
&sbrk,
&giveSCREEN,
&nop,
&nop,
&nop, 			//20
&nop,
&nop,
&nop,
&nop,
&_init_VESA,	//25
&nop,
&nop,
&nop,
&nop,
&access,
&ioctl,
&nop,			//32
&nop,
&nop,
&nop,
&sti
};

 void syscall_handler(struct regs *r)
{
    if (r->eax >= sizeof(syscalls)/sizeof(*syscalls))
        return;

  
    void* addr = syscalls[r->eax];

    __asm__ __volatile__(
     "push %1; \
      push %2; \
      push %3; \
      push %4; \
      push %5; \
      call *%6; \
      add $20, %%esp;"
       : "=a" (r->eax) : "D" (r->edi), "S" (r->esi), "d" (r->edx), "c" (r->ecx), "b" (r->ebx), "a" (addr));   	
}

void _install_syscall_handler()
{
	install_device(0x80,syscall_handler);
}
