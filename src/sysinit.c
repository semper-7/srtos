#include "stm32f1xx.h"
#include "sysinit.h"

void SystemInit (void)
{
  /* Set RCC initial state */
  RCC->CR   = RCC_CR_RESET_STATE;
  RCC->CFGR = 0;
  RCC->CIR  = 0;
  /* Enable HSE */
  RCC->CR |= RCC_CR_HSEON;
  /* Wait till HSE is ready */
  while (!(RCC->CR & RCC_CR_HSERDY));
  /* Enable Prefetch Buffer, set Flash memory wait state */
#if SYSCLK > 48000000
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;
#elif SYSCLK > 24000000
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1;
#else
  FLASH->ACR = FLASH_ACR_PRFTBE;
#endif
  /* HCLK = PCLK1 = PCLK2 = SYSCLK = HSE * PLL_MUL*/
  RCC->CFGR = RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV2 |
              RCC_CFGR_PLLSRC    | RCC_PLL(NUM_MUL);
  /* Enable PLL */
  RCC->CR |= RCC_CR_PLLON;
  /* Wait till PLL is ready */
  while(!(RCC->CR & RCC_CR_PLLRDY));
  /* Select PLL as system clock source */
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
  /* Wait till PLL is used as system clock source */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1);
}
