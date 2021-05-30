#ifndef __SYSINIT_H__
#define __SYSINIT_H__

#define RCC_CR_RESET_STATE 0x00000083ul
#define HSE_CLK 8000000
#define NUM_MUL 9
#define RCC_PLL(n) RCC_PLL_(n)
#define RCC_PLL_(n) RCC_CFGR_PLLMULL##n
#define SYSCLK HSE_CLK * NUM_MUL

#endif /*__SYSINIT_H__ */
