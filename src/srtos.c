#include "srtos.h"
#include "sysinit.h"
#include <string.h>

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

uint8_t getTaskId(char* name)
{
  int i;
  for (i=0; i<TSK; i++)
  {
    if (!strcmp(task[i].name, name)) break;
  }  
  return i;
}

char* getTaskName(uint8_t id)
{
  return task[id].name;
}

uint16_t getTaskStat(uint8_t id)
{
  return task[id].stat_top;
}

uint32_t isTask(uint8_t id)
{
  return task[id].stack_pointer;
}

void delay(uint32_t time_ms)
{
  task[task_act].timer = time_ms;
  while(task[task_act].timer);
}

void removeTask(uint8_t id)
{
  __set_BASEPRI(192);
  task[id].stack_pointer = 0;
  __set_BASEPRI(0);
}

static void exitTask(void)
{
  removeTask(task_act);
  while(1);
}

uint8_t addTask(char* name, void (*addr)(), uint32_t timer)
{
  int i = 0;
  __set_BASEPRI(192);
  while (++i < TSK)
  {
    if (!task[i].stack_pointer)
    {
      uint32_t sp = (uint32_t)&__stack_top__ - ((i+1)<<POW_STACK);
      *((uint32_t*)sp - 1) = PSR_RESET_VALUE;
      *((uint32_t*)sp - 2) = (uint32_t)addr;
      *((uint32_t*)sp - 3) = (uint32_t)exitTask;
      task[i].stack_pointer = sp - 64;
      task[i].timer = timer;
      task[i].skip_counter = 0;
      task[i].name = name;
      break;
    }
  }
  __set_BASEPRI(0);
  return i;
}

uint32_t selectTask(uint32_t sp)
{
  int skip = 0;
  int a = 0;
  int i = 0;

  if (task[task_act].stack_pointer) task[task_act].stack_pointer = sp;

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
          a = i;
          skip = task[i].skip_counter;
        } 
      }
      task[i].skip_counter++;
    }
  }
  task[a].skip_counter = 0;
  task[a].stat++;
  task_act = a;
  return task[a].stack_pointer;
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

void SysTick_Handler(void)
{
  static uint32_t sys_tick_counter;
  sys_tick_counter++;
    if (sys_tick_counter == 1000)
    {
      sys_tick_counter = 0;
      for (int i=0; i<TSK; i++)
      {
        task[i].stat_top = task[i].stat;
        task[i].stat = 0;
      }  
    }
  SysTickCallback();
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void startRtos(void)
{
  uint32_t sp = (uint32_t)&__stack_top__ - (1<<POW_STACK);
  *((uint32_t*)sp - 1) = PSR_RESET_VALUE;
  *((uint32_t*)sp - 2) = (uint32_t)idleTask;
  *((uint32_t*)sp - 3) = (uint32_t)exitTask;
  __set_PSP(sp - 32);
  task[0].stack_pointer = sp - 64;
  task[0].name = "idle";
  NVIC_SetPriority(SysTick_IRQn, 255);
  NVIC_SetPriority(PendSV_IRQn, 255);
  SysTick->LOAD  = SYSCLK/1000 - 1;  
  SysTick->VAL   = 0;
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk   | 
                   SysTick_CTRL_ENABLE_Msk;    
  __enable_irq();
  __enable_fault_irq();
  while(1);
}
