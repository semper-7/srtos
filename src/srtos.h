#ifndef __SRTOS_H__
#define __SRTOS_H__
#include "stm32f10x.h"

#define TICKS 72000     // count ticks in 1ms
#define TMR 8           // count of timer
#define TSK 4           // count of task    
#define POW_MSP 10      // the power of the number 2, size of msp = 2^10 = 1024
#define POW_PSP 8       // the power of the number 2, size of psp = 2^8  = 256


typedef struct TIMER {
  void (*callback)(void);
  uint32_t timer;
  uint32_t timer_auto;
  uint8_t *tick;
} TIMER;

typedef struct TASK {
  uint32_t stack_pointer;
  uint32_t priority;
  uint32_t counter;
  uint32_t delay;
} TASK;

uint32_t getLoadCPU();
void rtosInit(void);
void addTimer(void (*callback)(), uint32_t timer, uint32_t timer_auto, uint8_t* tick);
void delay(uint32_t time_ms);
uint32_t addTask(void (*addr_task)());

#endif  // __SRTOS_H__

