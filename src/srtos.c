#include "srtos.h"
#include "asmfunc.h"

TASK task[TSK];
TIMER tmr[TMR];
static volatile uint8_t task_new;
static volatile uint8_t timer_new;
static volatile uint32_t load_cpu;
static volatile uint32_t current_load;
volatile uint8_t flag_delay;
static void (*idle_callback)(void);
extern void *__stack_top__;


static void timerStat() {
  load_cpu=current_load;
  current_load=0;
}

uint32_t getLoadCPU() {
  return load_cpu;
}

uint8_t addTimer(void (*callback)(), uint32_t timer, uint32_t timer_auto) {
  __disable_irq();
  uint8_t n = timer_new++;
  if (n<TMR) {
    tmr[n].callback=callback;
    tmr[n].timer=timer;
    tmr[n].timer_auto=timer_auto;
  }
  __enable_irq();
  return n;
}

void rtosInit(void (*idle_func)()) {
  idle_callback=idle_func;
  SysTick_Config(TICKS);
  addTimer(timerStat,1000,1000);
}

void removeTimer(uint8_t num_tmr) {
  __memcpy(&tmr[num_tmr+1],&tmr[num_tmr],(sizeof(struct TIMER)) * (timer_new-num_tmr-1));
  timer_new--;
}

void SysTick_Handler(void) {
  for(int i=0; i<timer_new; i++) {
    if (tmr[i].timer) {
      tmr[i].timer--;
    } else {
      tmr[i].timer=tmr[i].timer_auto;
      if (tmr[i].callback) tmr[i].callback();
      if (!tmr[i].timer) removeTimer(i);
    }
  }
  current_load +=(TICKS-SysTick->VAL);
}

static void endDelayTimer(void) {
  flag_delay=0;
}

void delay(uint32_t time_ms) {
  addTimer(endDelayTimer,time_ms,0);
  flag_delay=1;
  while(flag_delay) {
    if (idle_callback) (*idle_callback)();
  }
}

uint32_t addTask(void (*addr_task)()) {
  __disable_irq();
  uint32_t n = task_new++;
  if (n<TSK) {
    uint32_t* sp = (uint32_t*)&__stack_top__ - 1024 - (n<<8);
    task[n].stack_pointer = (uint32_t)sp;
  }
  __enable_irq();
  return n;
}
