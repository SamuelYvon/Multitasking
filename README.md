# Multitasking
A very basic implementation of an operating system that can run multiple tasks "at the same time".

# The project

I was interested in how an operating system could execute multiple tasks at the same time, and I decided to implement it for myself to get a better understanding of what was going on. This is a software multitasking solution, that only uses one core of the CPU. This is of course not a viable production solution, since I simply started process stacks in random area of the memory. A more complete solution would of course make use of virtual memory to have the tasks executing in a seperate context. 

Writing this project gave me a much deeper understanding of x86 assembly and of course some basic operating system tasks. With the help of the kernel source I found, I also dove deeper into the boot process of a computer.

# Acknowledgement

This project uses a part of the source of the repository https://github.com/arjun024/mkeykernel. I used the starting code to quickly get a bootable kernel and setup the interrupts for the PIT. 

I only wanted to focus on the multitasking part and this is what arjun024's project allowed me to do, by offering a bootable kernel. I of course modified some elements, sometimes by personal taste, sometimes by requirement. 
