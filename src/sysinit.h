#ifndef __SYSINIT_H__
#define __SYSINIT_H__

#define RCC_CR_RESET_STATE 0x00000083ul
#define HSE_CLK 8000000

#ifdef SYSCLK_FREQ_24MHz
  #define PLL_MUL RCC_CFGR_PLLMULL3
  #define SYSCLK HSE_CLK * 3
#elif defined SYSCLK_FREQ_36MHz
  #define PLL_MUL RCC_CFGR_PLLMULL4
  #define SYSCLK HSE_CLK * 4
#elif defined SYSCLK_FREQ_40MHz
  #define PLL_MUL RCC_CFGR_PLLMULL5
  #define SYSCLK HSE_CLK * 5
#elif defined SYSCLK_FREQ_48MHz
  #define PLL_MUL RCC_CFGR_PLLMULL6
  #define SYSCLK HSE_CLK * 6
#elif defined SYSCLK_FREQ_56MHz
  #define PLL_MUL RCC_CFGR_PLLMULL7
  #define SYSCLK HSE_CLK * 7
#elif defined SYSCLK_FREQ_64MHz
  #define PLL_MUL RCC_CFGR_PLLMULL8
  #define SYSCLK HSE_CLK * 8
#elif defined SYSCLK_FREQ_72MHz
  #define PLL_MUL RCC_CFGR_PLLMULL9
  #define SYSCLK HSE_CLK * 9
#endif

#endif /*__SYSINIT_H__ */