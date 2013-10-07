#include <video.h>
#include <types.h>
#include <heapmngr.h>
#include <kutils.h>
#include <proc.h>
#include <sched.h>
volatile task_t *current_task;
volatile task_t *ready_queue;
volatile bool task_switching = true;
const u16 TIMER_HZ = 100;
void exit();
u32 counter = 0;
u32 volatile pid = 0;
#define TASK_RUNNING (1 << 0)
#define TASK_SLEEPING (1 << 1)
#define KERNEL_STACK_SIZE  8192
void _task_initialize(void)
{
	__asm__ __volatile__("cli");
	current_task = ready_queue = (task_t*)kmalloc(sizeof(task_t));
	memset((task_t *)current_task, 0, sizeof(task_t));
	current_task->id = pid++;
    current_task->esp = 0;
    current_task->eip = 0;
    current_task->privilege = 0;
    current_task->state = TASK_RUNNING;
    current_task->next = 0;
    current_task->type = THREAD;
    current_task->priority = PRIO_HIGH;
    current_task->time_to_run = 100;
    current_task->ready_to_run = 1;
	current_task->kernel_stack = (u32)kmalloc(KERNEL_STACK_SIZE)+KERNEL_STACK_SIZE;
	scheduler_install();
	insert_current_task(current_task);
	__asm__ __volatile__("sti");
}

void _get_task_stack(task_t *new_task,void (*entry)(),size_t argc, char** argv,u8 privilege, int priority)
{
	__asm__ __volatile__("cli");
	task_switching = false;
	memset(new_task, 0, sizeof(task_t));
	new_task->kernel_stack = kmalloc(KERNEL_STACK_SIZE)+KERNEL_STACK_SIZE;
	regs_t *kernel_stack = (regs_t*)kmalloc(sizeof(regs_t)+KERNEL_STACK_SIZE);
    
	new_task->state = TASK_RUNNING;
 
    task_t *tmp_task = (task_t*)ready_queue;
	while(tmp_task->next)
	{
		tmp_task = tmp_task->next;
	}
	tmp_task->next = new_task;
	new_task->privilege = privilege;

    u32 code_segment = 0x08, data_segment = 0x10;
    kernel_stack->ss = data_segment;
    if (new_task->privilege == 3)
        {         
           kernel_stack->ss = 0x23; 
           code_segment = 0x1B; 
        }
    if (new_task->privilege == 3)    
	kernel_stack->useresp = KERNEL_STACK_SIZE; 
	else
	kernel_stack->useresp =(u32)&exit;
	
	kernel_stack->eflags = 0x0202;
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
	insert_current_task(new_task);
	__asm__ __volatile__("sti");
}

s32 getpid()
{
	return current_task->id;
}

u32 _task_switch(u32 esp)
{
	
	if(!current_task) return esp;
		
	current_task->esp = esp;
	
	task_t *old_task = current_task;
	current_task = get_current_task();
	if(old_task == current_task)
		return esp;
		
	volatile task_t *t;
	for (t = ready_queue; t != 0; t = t->next){
		if (t->state == TASK_SLEEPING && t->wakeup_time <= gettickcount()){
				t->wakeup_time = 0;
				t->state = TASK_RUNNING;
		}
	}

	while (current_task != 0 && current_task->state == TASK_SLEEPING) 
          current_task = current_task->next;
	
	if(!current_task)
		current_task = ready_queue;
		
	set_kernel_stack(current_task->kernel_stack+KERNEL_STACK_SIZE);
	return current_task->esp;
}

void create_v86_task()
{
	
}

void create_user_task(void (*thread)())
{
	task_t* new_task = kmalloc(sizeof(task_t));
	_get_task_stack(new_task,thread,0,0,3,PRIO_LOW);
}

void create_kernel_task(void (*thread)(),int priority)
{
	task_t* new_task = kmalloc(sizeof(task_t));
	_get_task_stack(new_task,thread,0,0,0,priority);
}

void create_process()
{
	
}

void exit()
{
    __asm__ __volatile__("cli");
	current_task->priority = PRIO_DEAD;
	current_task->time_to_run = 0;
    current_task->ready_to_run = 0;

	delete_current_task(current_task);
  
    free((void *)((u32)current_task->kernel_stack - KERNEL_STACK_SIZE)); 
    free((void *)current_task);

    __asm__ __volatile__("sti");
	counter--;
	task_switching = 1; 
}

void sleep_task(u32 milliseconds) 
{
	const u32 start_ticks = gettickcount();
	u32 ticks_to_wait = milliseconds / (1000 / TIMER_HZ);

	if (ticks_to_wait == 0)
	ticks_to_wait = 1;
	
	current_task->state = TASK_SLEEPING;
	current_task->wakeup_time = start_ticks + ticks_to_wait;

	__asm__ __volatile__("int $0x20");
}

void task1()
{			
	kprint("Hello! from kernel task!\n");	
	for(;;);
}

void task3()
{	
	kprint("Hello! from user task! %d\n", getpid());
	for(;;);
}

void task2()
{
	for(;;);
}


int IdleTask(void)
{
	while(1)
	{
		if(getpid() == 4)
		 sleep_task(25);
		
		if(getpid() == 5)
		 sleep_task(1000);
		 
		if(getpid() == 6)
		 sleep_task(500);
		kprint("PID NUMBER FOR TASK %d\n", getpid());     
	}
	return 0;
}
void TASK_testing()
{
	create_user_task(task3);
	create_user_task(task3);
	create_kernel_task(task1,PRIO_IDLE);
	
	create_kernel_task(IdleTask,PRIO_IDLE);	
	create_kernel_task(IdleTask,PRIO_LOW);	
	create_kernel_task(IdleTask,PRIO_HIGH);	
}
