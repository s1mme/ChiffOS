#include <types.h>
#include <irq.h>
#include <video.h>
#include <vmmngr.h>
#include <elf.h>
#include <fat.h>
#include <proc.h>
#include <syscalls.h>
#include <kbd.h>
#define SBRK_START 0x1900000
#define SBRK_END 0x3000000
void cli()
{
	__asm__ volatile("cli");
}

int sti()
{
	__asm__ volatile("sti");
}

void *sbrk(int incr) {

static u32 brk = SBRK_START+SBRK_END;
int prev_brk = brk;
if (incr == 0) {
	return (void *)prev_brk;
}

if (incr > 0) {
	u32 new_end = brk + incr;
	new_end &= 0xfffff000;
	new_end += 0x1000;
	int i;

	for (i = SBRK_START; i < new_end; i += 0x1000) 
		_vmm_get_page_addr(i, 1, pd);
	for (i = SBRK_START; i <new_end; i += 0x1000)	
       _vmmngr_alloc_frame(_vmm_get_page_addr(i, 1, pd),0,1);

	brk = new_end;
	return (void *)prev_brk;
}
else {
	/* Caller wants to decrease the heap*/
	kprint("sbrk with negative argument: TODO");
	return 0;
	}
}

int fstat(int fd, struct stat *st) {
	
    if (!st)return -1;
    memset(st, 0, sizeof(struct stat));

    /*st->st_ino = current_task->fdtable[fd].node->inode;*/ 
    st->st_mode = 0666 | S_IFCHR; 
    st->st_size = current_task->fdtable[fd].size;
    st->st_blksize = 128;   
    return 0;
}

void *syscalls[] =
{
	&sbrk, /* 0 */
	&write,
	&read,
	&open,
	&fstat,
	&kputch, 	/* 5 */
	&execve,
	&exit,
	&getpid,
	&kill,
	&getchar,
	&stdio_read,
	&sti
};

void syscall_handler(regs_t *r)
{
	void *location = syscalls[r->eax];
/*	kprint("syscall number :%d\n",r->eax);*/
	  __asm__ volatile (
     "push %1; \
      push %2; \
      push %3; \
      push %4; \
      push %5; \
      call *%6; \
      add $20, %%esp;"
       : "=a" (r->eax) : "D" (r->edi), "S" (r->esi), "d" (r->edx), "c" (r->ecx), "b" (r->ebx), "a" (location));
}

void _install_syscall_handler()
{
	register_device(0x80,syscall_handler);
}
