#ifndef PROC_H_
#define PROC_H_

#include <types.h>
#include <fat.h>
typedef enum 
{
	THREAD, VM86
	
}task_type;


#define PRIO_DEAD 99
#define PRIO_IDLE 0
#define PRIO_LOW 1
#define PRIO_HIGH 2
#define CLUSTER_SIZE 512

typedef struct open_file {
	int count;
	u16 dev;
	u32  ino;
	u32  _cur_ino; 
	u32 offset;
	u32 size;
	FILE node[CLUSTER_SIZE];
	void *data; 
} open_file_t;

#define OPEN_MAX 16
struct task
{
	open_file_t fdtable[OPEN_MAX];
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
void create_process(void (*process)(),int priority,int argc, char** argv);
void exit();
#endif
