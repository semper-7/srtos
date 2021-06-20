#include "dwt.h"

void dwtInit(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void delay_us(uint32_t time_us)
{
  uint32_t t = time_us * TICKS_IN_US;
  DWT->CYCCNT = 0;
  while(DWT->CYCCNT < t);
}
