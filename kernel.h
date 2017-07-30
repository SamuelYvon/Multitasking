#ifndef KERNEL_HEADER
#define KERNEL_HEADER

typedef enum {
	TASK_WAITING_FOR_START,
	TASK_WAITING,
	TASK_RUNNING
} task_status_t;

typedef struct
{
	task_status_t status;
	unsigned int ebp;
	unsigned int esp;
	void *task;
} task_t;

typedef struct IDT_entry
{
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
} idt_entry_t;

void timer_handler();

char read_port(unsigned short port);

void write_port(unsigned short port, unsigned char data);

void load_idt(unsigned long *idt_ptr);

void switch_stack_and_jump(unsigned int stack, unsigned int task);

void switch_stack(unsigned int esp, unsigned int ebp);

#endif /*KERNEL_HEADER*/