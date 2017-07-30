//TODO: Since KPRINT uses shared ressources, we need mutexes!
//TODO: Mutexes

#include "keyboard.h"
#include "./lib/stdlib.h"
#include "kernel.h"

#define MAX_TASK 4

/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT *COLUMNS_IN_LINE *LINES

#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C

extern unsigned char keyboard_map[128];

/* current cursor location */
volatile unsigned int current_loc = 0;
/* video memory begins at address 0xb8000 */
char *vidptr = (char *)0xb8000;


idt_entry_t IDT[IDT_SIZE];

void configure_pic() {
	/* ICW1 - begin initialization */
	write_port(0x20, 0x11);
	write_port(0xA0, 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	write_port(0x21, 0x20);
	write_port(0xA1, 0x28);

	/* ICW3 - setup cascading */
	write_port(0x21, 0x00);
	write_port(0xA1, 0x00);

	/* ICW4 - environment info */
	write_port(0x21, 0x01);
	write_port(0xA1, 0x01);
	/* Initialization finished */

	/* mask interrupts */
	write_port(0x21, 0xff);
	write_port(0xA1, 0xff);
}

void configure_pit() {
	int freq = 1193182 / 100;
	write_port(0x43, 0b00110100);
	write_port(0x40, freq & 0xFFFF);
	write_port(0x40, (freq >> 8) & 0xFFFF);
}

void idt_init(void)
{
	unsigned long timer_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];
	
	timer_address = (unsigned long)timer_handler;
	IDT[0x20].offset_lowerbits = timer_address & 0xffff;
	IDT[0x20].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x20].zero = 0;
	IDT[0x20].type_attr = INTERRUPT_GATE;
	IDT[0x20].offset_higherbits = (timer_address & 0xffff0000) >> 16;

	configure_pic();

	configure_pit();

	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT;
	idt_ptr[0] = (sizeof(struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16;

	//Enable only IRQ0
	write_port(0x21, 0b11111110); // or ~1
	load_idt(idt_ptr);
}

void kprint(const char *str)
{
	unsigned int i = 0;
	while (str[i] != '\0')
	{
		vidptr[current_loc++] = str[i++];
		vidptr[current_loc++] = 0x07;
	}
}

void kprint_char(char to_print) {	
	vidptr[current_loc++] = to_print;
	vidptr[current_loc++] = 0x07;
}

void kprint_newline(void)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}

void clear_screen(void)
{
	unsigned int i = 0;
	while (i < SCREENSIZE)
	{
		vidptr[i++] = ' ';
		vidptr[i++] = 0x07;
	}
}

void task_first()
{
	int j = 0;
	while (1)
	{
		kprint(" ");
		kprint("T:");
		char arr[10];

		int length = int_to_str(&arr, 10, j);

		for(int i = 0; i < length; ++i) {
			kprint_char(arr[i]);
		}

		kprint(" ");

		for (int i = 0; i < 1000000; ++i)
		{
			__asm__("NOP");
		}
		++j;
	}
}

void task_second()
{
	int j = 1;
	while (1)
	{
		j += 2;
		kprint("2");
		for (int i = 0; i < 1000000; ++i)
		{
			__asm__("NOP");
		}
	}
}

void task_third()
{
	int j = 1;
	while (1)
	{
		j += 2;
		kprint("3");

		for (int i = 0; i < 1000000; ++i)
		{
			__asm__("NOP");
		}
	}
}

void task_fourth() {	
	int j = 1;
	while (1)
	{
		j += 2;
		kprint("4");
		for (int i = 0; i < 1000000; ++i)
		{
			__asm__("NOP");
		}
	}
}

static volatile int current_task_idx;
static task_t tasks[MAX_TASK];

/*
This is called from the assembler.
It happens when a timer event is fired by the programmable interval timer.
It switches the current process every 100hz. 
It takes as parameter the last EBP register value and the last ESP register value.

This method cannot be interrupted again. It is important to 

The ebp is the Extended Base Pointer. It points to the beginning of the stack frame
The esp is the Extended Stack Pointer. It poins to the end of the stack frame.

A low-level stack like the one in C is inverted. That means that the top of the stack will take the higher adress (ie 8000), and the 
end of the stack frame will be maybe at 7950. This is why EBP will always be bigger than ESP. In a 
java stack for instance, we would see the reverse. The stack frame base would always have a lower address than the
top of the stack (ESP).

With the EBP and ESP we receive, we save it into the stack. The rest of the registers has been 
saved by the PUSHA instruction. Since we do not deal with virtual memory, we do not have to worry
about the memory segment registers. Since the code is all in the same area, the CS register 
is also left to the default version.
*/
void timer_handler_main(unsigned int last_ebp, unsigned int last_esp)
{
	write_port(0x20, 0x20); //write the EOI. This allows another interrupt to come in later

	int old_task_idx = current_task_idx;
	int task_idx = ++current_task_idx;
	task_idx %= MAX_TASK;
	current_task_idx = task_idx;

	task_t *old_task = &tasks[old_task_idx];

	if (old_task->status == TASK_RUNNING)
	{
		old_task->ebp = last_ebp; // save the status of the task
		old_task->esp = last_esp; // all the registers are saved on the stack, so its the only important value to get
	}

	task_t *task_to_run = &tasks[current_task_idx];

	//If the task never ran, we want to JMP to it
	if (task_to_run->status == TASK_WAITING_FOR_START)
	{
		task_to_run->status = TASK_RUNNING;
		/*
		This is defined in the assembler too. 
		It will switch the stack currently being executed, and 
		will jump to the task's function pointer.
		*/
		switch_stack_and_jump(task_to_run->esp, task_to_run->task);
	}

	/*
	This will switch the currently executed stack to the stack of the
	next scheduled process. 

	It does not have to jmp anywhere, since the last task was
	interrupted, we simply have to do an iretd. This will go up the stack
	to find the last EIP (extended instruction pointer).
	*/
	switch_stack(task_to_run->esp, task_to_run->ebp);

	return;
}

static void create_task(int taskIdx, void *method, char *stack)
{
	task_t task;
	task.status = TASK_WAITING_FOR_START;
	task.ebp = stack; //creates a new stack
	task.esp = stack;
	task.task = method;

	tasks[taskIdx] = task;
}

void kernel_main(void)
{
	const char *str = "MULTITASKING TEST";
	
	clear_screen();
	kprint(str);
	kprint_newline();

	idt_init();

	create_task(0, (void *)task_first, (char *) 4000);
	create_task(1, (void *)task_second, (char *)6000);
	create_task(2, (void *)task_third, (char *) 2000);
	create_task(3, (void *)task_fourth, (char *)8000);

	while (1) {
		__asm__("NOP");
	}
}