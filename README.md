# Linux Project - add a new syscall
## Description
- Add a new system call `void linux_survey_TT(char *)` that you can call in your program.
- Parameter: the address of a memory area that can store all information the system call collects in the kernel. You may need to design the layout of the memory area to store your imformation.
- The system call records the **virtual address intervals of the user address space of the process** executing the system call.
- The system call records the **corresponding physical address intervals used by the above virtual address intervals** at the moment that you executed the system call.
- Write a program as `test.c` to collects the information you need.
