#ifndef __SRTOS_H__
#define __SRTOS_H__
#include "stm32f10x.h"

#define TICKS 72000             // count ticks in 1ms
#define TSK 8                   // count of task

typedef struct TASK {
  uint32_t timer;
  uint32_t timer_auto;
  void (*callback)(void);
} TASK;

uint32_t getLoadCPU();
void rtosInit(void(*idle_func)());
uint8_t addTask(void (*callback)(), uint32_t timer, uint32_t timer_auto);
void removeTask(uint8_t num_task);
void delay(uint32_t time_ms);

#endif  // __SRTOS_H__

