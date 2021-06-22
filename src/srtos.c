#include "srtos.h"
#include "sysinit.h"
#include <string.h>

static volatile TASK task[TSK];
static volatile uint8_t task_act;
static volatile uint8_t task_max;
static volatile uint8_t flag_stat;

extern void *__stack_top__;

void __attribute__((weak)) SysTickCallback(void) {}

uint8_t getTaskId(char* name)
{
  int i;
  for (i = 0; i <= task_max; i++)
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
  for (i = 0; i < TSK; i++)
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
      if (i > task_max) task_max = i;
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

  if (task[task_act].stack_pointer) task[task_act].stack_pointer = sp;

  for (int i = 0; i <= task_max; i++)
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
      if (flag_stat)
      {
        task[i].stat_top = task[i].stat;
        task[i].stat = 0;
      }
    }
  }
  flag_stat = 0;
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
      flag_stat = 1;
    }
  SysTickCallback();
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void startRtos(void)
{
  __set_PSP((uint32_t)&__stack_top__ - (1<<POW_STACK) - 32);
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
