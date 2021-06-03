#include "srtos.h"
#include "sysinit.h"
//#include <string.h>

static volatile TASK task[TSK];
static volatile uint32_t task_num;
static volatile uint32_t task_next;
static volatile uint8_t task_new;

extern void *__stack_top__;

void idleTask(void)
{
  while(1);
}

void rtosInit(void)
{
  addTask(idleTask,0);
  __set_PSP((uint32_t)&__stack_top__ - (1<<POW_MSP) - 32);
}

void delay(uint32_t time_ms)
{
  task[task_num].timer = time_ms;
  while(task[task_num].timer);
}

uint32_t addTask(void (*addr_task)(), uint32_t timer)
{
  uint32_t i = task_new++;
  if (i<TSK)
  {
    uint32_t sp = (uint32_t)&__stack_top__ - (1<<POW_MSP) - (i<<POW_PSP);
    *((uint32_t*)sp - 1) = PSR_RESET_VALUE;
    *((uint32_t*)sp - 2) = (uint32_t)addr_task;
    *((uint32_t*)sp - 3) = LR_RESET_VALUE;;
    task[i].stack_pointer = sp - 64;
    task[i].timer = timer;
    task[i].skip_counter = 0;
    task[i].wait_flag = 0;
  }
  return i;
}

uint32_t selectTask(uint32_t sp)
{
  uint32_t skip = 0;
  int tsk = 0;

  task[task_num].stack_pointer = sp;

  for(int i=1; i<task_new; i++)
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
  task[tsk].skip_counter = 0;
  task_num = tsk;
  return task[tsk].stack_pointer;
}

void PendSV_Handler(void)
{
  __asm volatile
  (
    "	mrs	r0, psp			\n"
    "	isb				\n"
    "	stmdb	r0!, {r4-r11}		\n"
    "	bl	selectTask		\n"
    "	ldmia	r0!, {r4-r11}		\n"
    "	msr	psp, r0			\n"
    "	isb				\n"
    "	mvn	lr, #2			\n"
    "	bx	lr			\n"
  );
}

void SVC_Handler(void)
{
  __asm volatile
  (
    "	isb				\n"
    "	mvn	lr, #2			\n"
    "	bx	lr			\n"
  );
}

void SysTick_Handler(void)
{
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void rtosStart(void)
{
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
  __SVC0();
  __NOP();
}
