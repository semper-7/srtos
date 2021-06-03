#include "srtos.h"
#include "asmfunc.h"
#include "usart1.h"
#include "func.h"
#include <string.h>

static volatile TASK task[TSK];
TIMER tmr[TMR];
static volatile uint32_t task_num;
static volatile uint8_t task_new;
static volatile uint8_t timer_new;
static volatile uint32_t load_cpu;
static volatile uint32_t current_load;

extern void *__stack_top__;
char usart_tx_bufer_rtos[80];

static void timerStat()
{
  load_cpu=current_load;
  current_load=0;
}

uint32_t getLoadCPU()
{
  return load_cpu;
}

void addTimer(void (*callback)(), uint32_t timer, uint32_t timer_auto, uint8_t *flag)
{
  __disable_irq();
  uint8_t n = timer_new;
  if (n<TMR)
  {
    tmr[n].callback = callback;
    tmr[n].timer = timer;
    tmr[n].timer_auto = timer_auto;
    tmr[n].flag = flag;
    timer_new++;
  }
  __enable_irq();
}

void rtosInit()
{
  NVIC_SetPriority (SysTick_IRQn, 255);
  NVIC_SetPriority (PendSV_IRQn, 255);
  SysTick->LOAD  = TICKS - 1;  
  SysTick->VAL   = 0;
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk   | 
                   SysTick_CTRL_ENABLE_Msk;    
  addTimer(timerStat,1000,1000,0);
}

static void removeTimer(uint8_t num_tmr)
{
  memcpy(&tmr[num_tmr+1],&tmr[num_tmr],(sizeof(struct TIMER)) * (timer_new-num_tmr-1));
  timer_new--;
}

void delay(uint32_t time_ms)
{
  uint32_t n = task_num;
  task[n].idle = 1;
  addTimer(0,time_ms,0,(uint8_t*)&task[n].idle);
  while(task[n].idle);
}

uint32_t addTask(void (*addr_task)())
{
  __disable_irq();
  uint32_t n = task_new++;
  if (n<TSK)
  {
     uint32_t sp = (uint32_t)&__stack_top__ - (1<<POW_MSP) - (n<<POW_PSP);
     *((uint32_t*)sp - 1) = PSR_RESET_VALUE;
     *((uint32_t*)sp - 2) = (uint32_t)addr_task;
     *((uint32_t*)sp - 3) = LR_RESET_VALUE;;
     task[n].stack_pointer = sp - 64;
  }
  __enable_irq();
  return n;
}

void SysTick_Handler(void)
{
  for(int i=0; i<timer_new; i++)
  {
    if (tmr[i].timer)
    {
      tmr[i].timer--;
    }
    else
    {
      tmr[i].timer=tmr[i].timer_auto;
      if (tmr[i].callback) tmr[i].callback();
      if (tmr[i].flag) *(tmr[i].flag) = 0;
      if (!tmr[i].timer) removeTimer(i);
    }
  }
  current_load +=(TICKS-SysTick->VAL);
  if (task_new > 1) SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void PendSV_Handler(void)
{
  task[task_num].stack_pointer = __push_r4_11(__get_PSP());
  task_num++;
  if (task_num >= task_new) task_num = 0;
  __set_PSP(__pop_r4_11(task[task_num].stack_pointer));
  __ISB();
  __set_BASEPRI(0);
}

void SVC_Handler(void)
{
  __set_PSP(__pop_r4_11(task[0].stack_pointer));
  __ISB();
  __set_BASEPRI(0);
  __set_LR(-3);
}

void rtosStart(void)
{
  __enable_irq();
  __enable_fault_irq();
  __DSB();
  __ISB();
  __SVC0();
  __NOP();
}
 
