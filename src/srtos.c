#include "srtos.h"
#include "asmfunc.h"


static TIMER tmr[TMR];
static volatile uint32_t load_cpu;
static volatile uint32_t current_load;
volatile uint8_t flag_delay;
static void (*idle_callback)(void);

static void tmrStat() {
  load_cpu=current_load;
  current_load=0;
}

uint32_t getLoadCPU() {
  return load_cpu;
}

uint8_t addTimer(void (*callback)(), uint32_t timer, uint32_t timer_auto) {
  int r = 0;
  for(int i=0; i<TMR; i++) {
    if (tmr[i].callback == 0) {
      tmr[i].callback=callback;
      tmr[i].timer=timer;
      tmr[i].timer_auto=timer_auto;
      r = i;
      break;
    }
  }
  return r;
}

void rtosInit(void (*idle_func)()) {
  idle_callback=idle_func;
  SysTick_Config(TICKS);
  addTimer(tmrStat,1000,1000);
  __enable_irq();
}

void removeTimer(uint8_t num_tmr) {
  _memcpy(&tmr[num_tmr+1],&tmr[num_tmr],(sizeof(struct TIMER)) * (TMR-1-num_tmr));
  _bzero(&tmr[TMR-1],sizeof(struct TIMER));
}

void SysTick_Handler(void) {
  for(int i=0; i<TMR; i++) {
    if (tmr[i].callback == 0) break;

    if (!tmr[i].timer) {
      tmr[i].timer=tmr[i].timer_auto;
      tmr[i].callback();
      if (!tmr[i].timer) removeTimer(i);
      break;
    } else tmr[i].timer--;
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
