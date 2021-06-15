# Simple RTOS for STM32F1XX
Example of a project using a Simple RTOS for stm32f1xx. The CMSIS in the project is derived from STM32CubeMX.
To use SRTOS in your projects, copy the following files to your sources: srtos.h, srtos.c and SYSCLK must be defined.
Note: SRTOS uses the upper RAM addresses for the main stack and the task stack.
The stacks size and the maximum number of tasks are defined in srtos.h.
To use the idle time, insert the idleTask() into your program (note: do not use the delay function in idleTask). To use the Sytick callback function, insert the sytickcallback() function into your program.

## Clone and compiling (Windows)
1. Install ARM GCC toolchain from https://gnutoolchains.com/arm-eabi/
2. Edited make.cmd line: set PATH=%PATH%;D:\ARM-GCC\bin\
3. Go make.cmd 
