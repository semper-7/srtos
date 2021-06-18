# Simple RTOS for STM32F1XX
Example of a project using a Simple RTOS for stm32f1xx.
Simple RTOS implements preemptive multitasking with a period of 1 ms.
The priority of the task is determined by a simple high-speed algorithm. The task that has been waiting for a long time is the highest priority. If the waiting periods are equal, the task with the lower number in the task table is given priority.
To use SRTOS in your projects, copy the following files to your sources: srtos.h, srtos.c and SYSCLK must be defined.
The CMSIS in the project is derived from STM32CubeMX.  
Note: SRTOS uses the upper RAM addresses for the task stack.
The stacks size and the maximum number of tasks are defined in srtos.h.
To use the idle time, insert the idleTask() into your program (note: do not use the delay function in idleTask). To use the SysTick callback function, insert the SysTickCallback() function into your program.
To add tasks, use addTask(), to complete tasks, just let the task exit (organize the final loop, see main.c in the project).
The timer parameter in addTask() defines the delay of the task execution and can be used for a single timer task (just don't loop the task).
It is also possible to view task statistics (see main.c statTask())

## Compiling (in Windows)
1. Install ARM GCC toolchain from https://gnutoolchains.com/arm-eabi/
2. Edited make.cmd line: set PATH=%PATH%;D:\ARM-GCC\bin\
3. Run make.cmd

## Flash (in Windows via uart)
1. Install Flash Loader Demo from https://www.st.com
2. Edited flash.cmd line: set P="C:\Program Files\STMicroelectronics\Software\Flash Loader Demo"
3. Run flash.cmd
