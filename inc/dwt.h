#ifndef __DWT_H__
#define __DWT_H__

#include "stm32f1xx.h"
#include "sysinit.h"

inline void dwtInit(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

inline void delay_us(uint32_t time_us)
{
  uint32_t t = time_us * (SYSCLK / 1000000);
  DWT->CYCCNT = 0;
  while(DWT->CYCCNT < t);
}

inline void delay_ns(uint32_t time_ns)
{
  uint32_t t = (time_ns * (SYSCLK / 1000000) / 1000);
  DWT->CYCCNT = 0;
  while(DWT->CYCCNT < t);
}

#endif //__DWT_H__
