#include <types.h>
#include <irq.h>
#include <video.h>
void cli()
{
	__asm__ volatile("cli");
}

void *syscalls[2] =
{
	&cli,
	&kputs
};

void syscall_handler(regs_t *r)
{
	void *location = syscalls[r->eax];
	kprint("syscall number :%d\n",r->eax);
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
