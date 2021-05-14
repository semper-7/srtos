#ifndef __SRTOS_H__
#define __SRTOS_H__
#include "stm32f10x.h"

#define TICKS 72000                // count ticks in 1ms
#define TMR 8                      // count of timer
#define TSK 4                      // count of task

typedef struct TIMER {
  void (*callback)(void);
  uint32_t timer;
  uint32_t timer_auto;
} TIMER;

typedef struct TASK {
  uint32_t context[16];
} TASK;

uint32_t getLoadCPU();
void rtosInit(void(*idle_func)());
uint8_t addTimer(void (*callback)(), uint32_t timer, uint32_t timer_auto);
void removeTimer(uint8_t num_tmr);
void delay(uint32_t time_ms);

#endif  // __SRTOS_H__

