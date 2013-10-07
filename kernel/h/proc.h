#ifndef PROC_H_
#define PROC_H_

#include <types.h>
typedef enum 
{
	THREAD, VM86
	
}task_type;
#define PRIO_DEAD 99
#define PRIO_IDLE 0
#define PRIO_LOW 1
#define PRIO_HIGH 2
struct task
{
    int priority;
    /*page_directory_t* directory;*/
    u32 time_to_run;       /* Time left on quanta*/
    int time_running;      /*Time spent running*/
    int ready_to_run;
	task_type type;
	s32 id;
	u32 ss;
	u8 privilege;
	void* FPUptr; 
	u32 eip;
	u32 esp;
	char *name;
	u32 kernel_stack;
	struct task* next;
	u32 state;
	u32 wakeup_time;	
}__attribute__((packed));

typedef struct task task_t;
extern volatile bool task_switching;
extern volatile task_t *current_task;
extern volatile task_t *ready_queue;
void _task_initialize();
#endif
