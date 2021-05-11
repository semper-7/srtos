#include "srtos.h"
#include "asmfunc.h"


static TASK task[TSK];
static volatile uint32_t load_cpu;
static volatile uint32_t current_load;
volatile uint8_t flag_delay;
static void (*idle_callback)(void);

static void taskStat() {
  load_cpu=current_load;
  current_load=0;
}

uint32_t getLoadCPU() {
  return load_cpu;
}

uint8_t addTask(void (*callback)(), uint32_t timer, uint32_t timer_auto) {
  int r = 0;
  for(int i=0; i<TSK; i++) {
    if (task[i].callback == 0) {
      task[i].callback=callback;
      task[i].timer=timer;
      task[i].timer_auto=timer_auto;
      r = i;
      break;
    }
  }
  return r;
}

void rtosInit(void (*idle_func)()) {
  idle_callback=idle_func;
  SysTick_Config(TICKS);
  addTask(taskStat,1000,1000);
  __enable_irq();
}

void removeTask(uint8_t num_task) {
  _memcpy(&task[num_task+1],&task[num_task],(sizeof(struct TASK)) * (TSK-1-num_task));
  _bzero(&task[TSK-1],sizeof(struct TASK));
}

void SysTick_Handler(void) {
  for(int i=0; i<TSK; i++) {
    if (task[i].callback == 0) break;

    if (!task[i].timer) {
      task[i].timer=task[i].timer_auto;
      task[i].callback();
      if (!task[i].timer) removeTask(i);
      break;
    } else task[i].timer--;
  }
  current_load +=(TICKS-SysTick->VAL);
}

static void endDelayTask(void) {
  flag_delay=0;
}

void delay(uint32_t time_ms) {
  addTask(endDelayTask,time_ms,0);
  flag_delay=1;
  while(flag_delay) {
    if (idle_callback) (*idle_callback)();
  }
}
