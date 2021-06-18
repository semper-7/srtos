#ifndef __SRTOS_H__
#define __SRTOS_H__
#include "stm32f1xx.h"

#define TSK 8       // count of user task + idle task
#define POW_STACK 9 // the power of the number 2, size of the task stack = 2^9 = 512
#define PSR_RESET_VALUE 0x01000000 // Program status register reset value

typedef struct {
  uint32_t stack_pointer;
  uint32_t skip_counter;
  uint32_t timer;
  uint16_t stat;
  uint16_t stat_top;
  char *name;
} TASK;

void startRtos(void);
void delay(uint32_t time_ms);
void addTask(char* name, void (*addr)(), uint32_t timer);
char* getTaskName(uint8_t t);
uint16_t getTaskStat(uint8_t t);
uint32_t isTask(uint8_t t);

#endif  // __SRTOS_H__

