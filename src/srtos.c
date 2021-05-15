#include "srtos.h"
#include "asmfunc.h"

volatile TASK task[TSK];
TIMER tmr[TMR];
static volatile uint8_t task_new;
static volatile uint8_t timer_new;
static volatile uint32_t load_cpu;
static volatile uint32_t current_load;

extern uint8_t *__stack_top__;

static void timerStat() {
  load_cpu=current_load;
  current_load=0;
}

uint32_t getLoadCPU() {
  return load_cpu;
}

void addTimer(void (*callback)(), uint32_t timer, uint32_t timer_auto, uint8_t *tick) {
  __disable_irq();
  uint8_t n = timer_new;
  if (n<TMR) {
    tmr[n].callback = callback;
    tmr[n].timer = timer;
    tmr[n].timer_auto = timer_auto;
    tmr[n].tick = tick;
    timer_new++;
  }
  __enable_irq();
}

void rtosInit() {
  SysTick_Config(TICKS);
  addTimer(timerStat,1000,1000,0);
}

static void removeTimer(uint8_t num_tmr) {
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
      if (tmr[i].tick) (*(tmr[i].tick))++;
      if (!tmr[i].timer) removeTimer(i);
    }
  }
  current_load +=(TICKS-SysTick->VAL);
}

uint32_t getNumberTask(void) {
  return 0; 
}

void delay(uint32_t time_ms) {
  uint32_t n = getNumberTask();
  task[n].delay = 0;
  addTimer(0,time_ms,0,(uint8_t*)&task[n].delay);
  while(!task[n].delay);
}

uint32_t addTask(void (*addr_task)()) {
  __disable_irq();
  uint32_t n = task_new++;
  if (n<TSK) {
     uint32_t sp = (uint32_t)&__stack_top__ - (1<<POW_MSP) - (n<<POW_PSP);
     *((uint32_t*)sp - 1) = 0x01000000;
     *((uint32_t*)sp - 2) = (uint32_t)addr_task;
     __bzero((uint32_t*)sp - 8, 24);
  }
  __enable_irq();
  return n;
}
