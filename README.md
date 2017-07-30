# Multitasking
A very basic implementation of an operating system that can run multiple tasks "at the same time".

# The project

I was interested in how an operating system could execute multiple tasks at the same time, and I decided to implement it for myself to get a better understanding of what was going on. This is a software multitasking solution, that only uses one core of the CPU. This is of course not a viable production solution, since I simply started process stacks in random area of the memory. A more complete solution would of course make use of virtual memory to have the tasks executing in a seperate context. 

Writing this project gave me a much deeper understanding of x86 assembly and of course some basic operating system tasks. With the help of the kernel source I found, I also dove deeper into the boot process of a computer.

# Executing and building the project

## Requirements

- GCC
- NASM assembler
- QEMU i386

## Howto

Simply run ./build_and_run.sh to, like the script says, build and run the project. It will require qemu-system-i386 to run. The kernel will not run until you attach a debugger to it. You can simply attach with GDB to port :1234 (the default port of qemu). If you want to start it without debugging, you can remove the -S option in the ./build_and_run.sh script. You can also remove the -s to remove debugging all together.

I used visual studio code to work on this project, with the native debugging extension to connect to GDB. It can be useful to see what is going on and make modifications to the kernel.

# Acknowledgement

This project uses a part of the source of the repository https://github.com/arjun024/mkeykernel. I used the starting code to quickly get a bootable kernel and setup the interrupts for the PIT. 

I only wanted to focus on the multitasking part and this is what arjun024's project allowed me to do, by offering a bootable kernel. I of course modified some elements, sometimes by personal taste, sometimes by requirement. 
