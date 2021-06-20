#ifndef __DWT_H__
#define __DWT_H__

#include "stm32f1xx.h"
#include "sysinit.h"

#define TICKS_IN_US (SYSCLK / 1000000u)

void dwtInit(void);
void delay_us(uint32_t time_us);

#endif //__DWT_H__
