#include "srtos.h"
#include "sysinit.h"
//#include <string.h>

static volatile TASK task[TSK];
static volatile uint32_t task_act;

extern void *__stack_top__;

void __attribute__((weak)) idleTask(void)
{
  while(1);
}

void __attribute__((weak)) SysTickCallback(void)
{
}

void delay(uint32_t time_ms)
{
  task[task_act].timer = time_ms;
  while(task[task_act].timer);
}

uint32_t addTask(void (*addr_task)(), uint32_t timer)
{
  int i = 0;
  __set_BASEPRI(192);
  while (++i < TSK)
  {
    if (!task[i].stack_pointer)
    {
      uint32_t sp = (uint32_t)&__stack_top__ - (1<<POW_MSP) - (i<<POW_PSP);
      *((uint32_t*)sp - 1) = PSR_RESET_VALUE;
      *((uint32_t*)sp - 2) = (uint32_t)addr_task;
      *((uint32_t*)sp - 3) = LR_RESET_VALUE;
      task[i].stack_pointer = sp - 64;
      task[i].timer = timer;
      task[i].skip_counter = 0;
      task[i].wait_flag = 0;
      break;
    }
  }
  __set_BASEPRI(0);
  return i;
}

uint32_t selectTask(uint32_t sp)
{
  int skip = 0;
  int tsk = 0;
  int i = 0;

  task[task_act].stack_pointer = sp;

  while (++i < TSK)
  {
    if (task[i].stack_pointer)
    {
      if (task[i].timer)
      {
      task[i].timer--;
      }
      else
      {
        if (task[i].skip_counter > skip)
        {
        tsk = i;
        skip = task[i].skip_counter;
        } 
      }
      task[i].skip_counter++;
    }
  }
  task[tsk].skip_counter = 0;
  task_act = tsk;
  return task[tsk].stack_pointer;
}

void PendSV_Handler(void)
{
  __asm volatile
  (
    "	mrs	r0, psp		\n"
    "	isb			\n"
    "	stmdb	r0!, {r4-r11}	\n"
    "	bl	selectTask	\n"
    "	ldmia	r0!, {r4-r11}	\n"
    "	msr	psp, r0		\n"
    "	isb			\n"
    "	mvn	lr, #2		\n"
    "	bx	lr		\n"
  );
}

void SVC_Handler(void)
{
  __asm volatile
  (
    "	isb			\n"
    "	mvn	lr, #2		\n"
    "	bx	lr		\n"
  );
}

void SysTick_Handler(void)
{
  SysTickCallback();
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void rtosStart(void)
{
  uint32_t sp = (uint32_t)&__stack_top__ - (1<<POW_MSP);
  *((uint32_t*)sp - 1) = PSR_RESET_VALUE;
  *((uint32_t*)sp - 2) = (uint32_t)idleTask;
  *((uint32_t*)sp - 3) = LR_RESET_VALUE;
  __set_PSP(sp - 32);
  task[0].stack_pointer = sp - 64;
  NVIC_SetPriority (SysTick_IRQn, 255);
  NVIC_SetPriority (PendSV_IRQn, 255);
  SysTick->LOAD  = SYSCLK/1000 - 1;  
  SysTick->VAL   = 0;
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk   | 
                   SysTick_CTRL_ENABLE_Msk;    
  __enable_irq();
  __enable_fault_irq();
  __DSB();
  __ISB();
  __SVC_0();
  __NOP();
}
