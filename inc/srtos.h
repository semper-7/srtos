#ifndef __SRTOS_H__
#define __SRTOS_H__
#include "stm32f1xx.h"

#define TSK 8       // count of user task + idle task
#define POW_STACK 9 // the power of the number 2, size of the task stack = 2^9 = 512
#define PSR_RESET_VALUE 0x01000000 // Program status register reset value

typedef struct TASK {
  uint32_t stack_pointer;
  uint32_t skip_counter;
  uint32_t timer;
  uint8_t* wait_flag;
} TASK;

void startRtos(void);
void delay(uint32_t time_ms);
void addTask(void (*addr_task)(), uint32_t timer);

inline uint32_t __get_MEM(uint32_t addr)
{
  uint32_t value;
  __asm volatile
  (
    "	ldr	%0,[%1]	" : "=r" (value) :"r" (addr) : "memory"
  );
  return value;
}

inline uint32_t __get_SP(void)
{
  uint32_t value;
  __asm volatile
  (
    "	mov	%0,sp	" : "=r" (value) : : "memory"
  );
  return value;
}

inline uint32_t __get_LR(void)
{
  uint32_t value;
  __asm volatile
  (
    "	mov	%0,lr	" : "=r" (value) : : "memory"
  );
  return value;
}

inline void __set_LR(uint32_t value)
{
  __asm volatile
  (
    "	mov	lr,%0	" : : "J" (value) : "memory"
  );
}

inline void __SVC_0(void)
{
  __asm volatile
  (
    "	svc	0	"
  );
}

inline uint32_t __push_r4_11(uint32_t value)
{
  __asm volatile
  (
    "	stmdb	%0!,{r4-r11}	" : "=r" (value) : "0" (value) : "memory"
  );
  return value;
}

inline uint32_t __pop_r4_11(uint32_t value)
{
  __asm volatile
  (
    "	ldmia	%0!,{r4-r11}	" : "=r" (value) : "0" (value) : "memory"
  );
  return value;
}

#endif  // __SRTOS_H__

