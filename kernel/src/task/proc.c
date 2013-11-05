#include <video.h>
#include <types.h>
#include <heapmngr.h>
#include <kutils.h>
#include <proc.h>
#include <sched.h>
#include <vesa.h>
#include <elf.h>
#include <fat.h>
#include <timer.h>
volatile task_t *current_task;
volatile task_t *ready_queue;

volatile bool task_switching = true;
extern uintptr_t vesa_com_start;
extern uintptr_t vesa_com_end;
const u16 TIMER_HZ = 100;
void exit();
u32 counter = 0;
u32 volatile pid = 0;

int execve(char *name, char **argv, char **env)
{
	int argc = 0;
	while (argv[argc]) { ++argc; }
	FILE elf;
	elf_header_t * elf_program;
	elf = ls_dir(name,0);
	elf_program = read_elf(elf);
	create_process((void*)elf_program->entry,THREAD,0,argc,argv);
	return 0;
}

#define KERNEL_STACK_SIZE  4096
void _task_initialize(void)
{
	__asm__ __volatile__("cli");
	current_task = ready_queue = (task_t*)malloc_(sizeof(task_t));
	memset((task_t *)current_task, 0, sizeof(task_t));
	current_task->id = pid++;
    current_task->esp = 0;
    current_task->eip = 0;
    current_task->privilege = 0;
    current_task->state = TASK_RUNNING;
    current_task->next = 0;
    current_task->type = THREAD;
    current_task->priority = PRIO_HIGH;
    current_task->time_to_run = 10;
    current_task->ready_to_run = 1;
	current_task->kernel_stack = (u32)malloc_a(KERNEL_STACK_SIZE)+KERNEL_STACK_SIZE;
	scheduler_install();
	insert_current_task(current_task);
	__asm__ __volatile__("sti");
}

u32 geteip()
{

return current_task->eip;
}

s32 getpid()
{
return current_task->id;
}

void _get_task_stack(task_t *new_task,void (*entry)(),size_t argc, char** argv,u8 privilege, int priority,task_type type)
{
	
	__asm__ __volatile__("cli");
	
	task_switching = false;
	memset(new_task, 0, sizeof(task_t));
	new_task->kernel_stack = (u32)malloc_a(KERNEL_STACK_SIZE)+KERNEL_STACK_SIZE;
	
    
	new_task->state = TASK_RUNNING;
	
		current_task->priority = priority;
	new_task->privilege = privilege;
	new_task->type = type;
	
	new_task->state = TASK_RUNNING;
	new_task->priority = priority;
	switch(new_task->priority)
	{
		case PRIO_DEAD:
			new_task->time_to_run = 0;
		break;
		case PRIO_IDLE:
			new_task->time_to_run = 3;
		break;
		case PRIO_LOW:
			new_task->time_to_run = 5;
		break;
		case PRIO_HIGH:
			new_task->time_to_run = 10;
		break;
		default:
			new_task->time_to_run = 10;
		break;
	}

    task_t *tmp_task = (task_t*)ready_queue;
	while(tmp_task->next)
	{
		tmp_task = tmp_task->next;
	}
	tmp_task->next = new_task;
	
	regs_t *kernel_stack = (regs_t*)malloc_(sizeof(regs_t));
	
    u32 code_segment = 0x08, data_segment = 0x10;
    u32 eflags = 0x0202;
    kernel_stack->useresp =(u32)&exit;
    kernel_stack->ss = data_segment;
    if (new_task->privilege == 3 && new_task->type == THREAD)
        {         
           kernel_stack->ss = 0x23; 
           kernel_stack->useresp = KERNEL_STACK_SIZE;
           code_segment = 0x1B; 
        }
	if(new_task->privilege == 3 && new_task->type == VM86)
		{
	     code_segment = 0;
	     kernel_stack->ss = 0x23;

		 kernel_stack->useresp = KERNEL_STACK_SIZE;
		 eflags = 0x20202;
		}	
		
	kernel_stack->eflags = eflags;
	kernel_stack->cs = code_segment;
	kernel_stack->eip = (u32)entry;
	kernel_stack->err_code = 0;
	kernel_stack->int_no = 0;
	kernel_stack->eax = argc;
	kernel_stack->ecx = (uintptr_t)argv;
	kernel_stack->edx = 0;
	kernel_stack->ebx = 0;
	kernel_stack->ebp = 0;
	kernel_stack->esi = 0;
	kernel_stack->edi = 0;

	if(privilege == 3) data_segment = 0x23;
	kernel_stack->ds = data_segment;
	kernel_stack->es = data_segment;
	kernel_stack->fs = data_segment;
	kernel_stack->gs = data_segment;
	
	new_task->eip = (u32)entry;
	new_task->esp = (u32)kernel_stack; 
	new_task->id = pid++;
	
	task_switching = true;
	insert_current_task(new_task);
	__asm__ __volatile__("sti");
	
}


void create_task_vm86(void (*thread)(),char *test)
{
	task_t* new_task = malloc_(sizeof(task_t));
	
	_get_task_stack(new_task,thread,0,0,3,PRIO_HIGH,VM86);	
   sleep2(40);
}

void create_user_task(void (*thread)())
{
	task_t* new_task = malloc_(sizeof(task_t));
	_get_task_stack(new_task,thread,0,0,3,PRIO_LOW,THREAD);
	sleep2(40);
}

void create_task_thread(void (*thread)(),int priority)
{
	task_t* new_task = malloc_(sizeof(task_t));
	_get_task_stack(new_task,thread,0,0,0,priority,THREAD);
	sleep2(40);
}

void create_process(void (*process)(),task_type type,u8 privilege, int argc, char** argv)
{
	task_t* new_task = malloc_(sizeof(task_t));
	_get_task_stack(new_task,process,argc,(uintptr_t)argv,privilege,PRIO_HIGH, type);
	sleep2(40);	
		 
}


u32 _task_switch(u32 esp)
{
if(!current_task) return esp;
current_task->esp = esp;
 task_t* oldTask = current_task; 
	current_task = get_current_task();
 	
 
 if(oldTask == current_task) return esp; // No task switch because old==new

if(current_task->priority == PRIO_LOW)
			current_task->time_to_run = 3;
   
        if( current_task->priority == PRIO_IDLE)
			current_task->time_to_run = 2;	
		
		 if( current_task->priority == PRIO_HIGH)
			current_task->time_to_run = 4;

for (volatile task_t *t = ready_queue; t != 0; t = t->next){
  if (t->state == TASK_SLEEPING && t->wakeup_time <= gettickcount()){
t->wakeup_time = 0;
t->state = TASK_RUNNING;
  }
  
}

while (current_task != 0 && current_task->state == TASK_SLEEPING) {
        
               current_task = current_task->next;
               
                }
if(!current_task)
{

current_task = ready_queue;

}

if (current_task == FPUTask)
 	{
 	__asm__ volatile("CLTS"); 
	}
 	else
 	{
 	u32 cr0;
 	__asm__ volatile("mov %%cr0, %0": "=r"(cr0));
 	cr0 |= BIT(3); 
	__asm__ volatile("mov %0, %%cr0":: "r"(cr0));
 	} 
	set_kernel_stack(current_task->kernel_stack+KERNEL_STACK_SIZE); //update the tss entry when we changes stack!
return current_task->esp;
}


void sleep2(u32 milliseconds) 
{
	const u32 start_ticks = gettickcount();
	u32 ticks_to_wait = milliseconds / (1000 / TIMER_HZ);

	if (ticks_to_wait == 0)
	ticks_to_wait = 1;
	
	current_task->state = TASK_SLEEPING;
	current_task->wakeup_time = start_ticks + ticks_to_wait;

	__asm__ __volatile__("int $0x20");
}


void switch_context() 
 	{
 	if(scheduler_shouldSwitchTask()) 
 	__asm__ volatile("int $0x20");
 	else
 	{
 	__asm__ volatile("hlt");
 	__asm__ volatile("int $0x20");
 	}
 	}
 



void exit()
{
    __asm__ __volatile__("cli");
	current_task->priority = PRIO_DEAD;
	current_task->time_to_run = 0;
    current_task->ready_to_run = 0;
	task_t* tmp_task = (task_t*)ready_queue;
    do
    {
        if(tmp_task->next == current_task)
        {
            tmp_task->next = current_task->next;
        }
        if(tmp_task->next)
        {
            tmp_task = tmp_task->next;
        }
    }
    while (tmp_task->next);
	delete_current_task(current_task);
  
    free((void *)((u32)current_task->kernel_stack - KERNEL_STACK_SIZE)); 
    free((void *)current_task);

    __asm__ __volatile__("sti");
	counter--;
	task_switching = 1; 
	switch_context();
}

void _exit(int status)
{
    __asm__ volatile("cli");
 current_task->priority = PRIO_DEAD;
current_task->time_to_run = 0;
     current_task->ready_to_run = 0;
      task_t* tmp_task = (task_t*)ready_queue;
    do
    {
        if(tmp_task->next == current_task)
        {
            tmp_task->next = current_task->next;
        }
        if(tmp_task->next)
        {
            tmp_task = tmp_task->next;
        }
    }
    while (tmp_task->next);
delete_current_task(current_task);
  
    //free((void *)((u32)current_task->kernel_stack - KERNEL_STACK_SIZE)); // free kernelstack
    //free((void *)current_task);

  
    
    __asm__ volatile("sti");
counter--;
task_switching = 1;
       // switch_context(); // switch to next task
    
}

void task01()
{			
	printk("Hello! from kernel task!\n");	
	sleep2(20000);
	for(;;);
}

void task02()
{	
	printk("Hello! from user task! %d\n", getpid());
	for(;;);
}

#define COM_ENTRY (void*)0x100
#define VESA_MODE 279
/*u16 *surface = 0;*/
void vesa_com_task()
{
	/*VESA with v86 task from COM file */
	/* *(u16*)0x3600 = VESA_MODE;
	memcpy(COM_ENTRY, &vesa_com_start, (u32)&vesa_com_end - (u32)&vesa_com_start);
	create_v86_task((void*)0x100);
	
	create_v86_task((void*)0x11d);
	sleep(1300);
	memcpy(&mib, (void*)0x3600, sizeof(VESA_MODE_INFO));
#ifdef DEBUGGING	
	kprint("PhysBasePtr: %x\n",mib.PhysBasePtr);
	
	kprint("XResolution: %d pixels \n",mib.XResolution);
	kprint("XResolution: %d pixels \n",mib.YResolution);
	kprint("BitsPerPixel: %d\n", mib.BitsPerPixel);
	kprint("WinSize %d\n", mib.WinSize);
#endif

	surface = paging_getVirtaddr(mib.PhysBasePtr, 0x400);

	memset(surface, 200, mib.XResolution*mib.YResolution*2);
*/
}

int IdleTask(void)
{
	
	while(1)
	{		
		if(getpid() == 4)
		 sleep2(25);
		
		if(getpid() == 5)
		 sleep2(1000);
		 
		if(getpid() == 6)
		 sleep2(500);
		printk("PID NUMBER FOR TASK %d\n", getpid());     
	}
	return 0;
}

void TASK_testing()
{	 
	
	create_task_thread(IdleTask,PRIO_HIGH);
	create_task_thread(IdleTask,PRIO_HIGH);
	create_user_task(IdleTask);
	create_user_task(IdleTask);
	create_task_thread(IdleTask,PRIO_HIGH);
	create_task_thread(IdleTask,PRIO_HIGH);
	/*vesa_task();*/
	
	/*
	create_kernel_task(IdleTask,PRIO_IDLE);	
	create_kernel_task(IdleTask,PRIO_LOW);	
	create_kernel_task(IdleTask,PRIO_HIGH);	*/
}

void kill(int pid) {
  volatile task_t *t;
  volatile task_t *prev = ready_queue;
  for (t = ready_queue; t != 0; prev = t, t = t->next) {
    if (t->id == pid) {
        prev->next = t->next;
      counter--;
    }
  }
}
