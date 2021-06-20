#ifndef __GPIO_H__
#define __GPIO_H__

#include "stm32f1xx.h"

#define GPIOA_CRL(MASK, MODE) MODIFY_REG( GPIOA->CRL, MASK, MODE );
#define GPIOB_CRL(MASK, MODE) MODIFY_REG( GPIOB->CRL, MASK, MODE );
#define GPIOC_CRL(MASK, MODE) MODIFY_REG( GPIOC->CRL, MASK, MODE );
#define GPIOD_CRL(MASK, MODE) MODIFY_REG( GPIOD->CRL, MASK, MODE );
#define GPIOE_CRL(MASK, MODE) MODIFY_REG( GPIOE->CRL, MASK, MODE );
#define GPIOA_CRH(MASK, MODE) MODIFY_REG( GPIOA->CRH, MASK, MODE );
#define GPIOB_CRH(MASK, MODE) MODIFY_REG( GPIOB->CRH, MASK, MODE );
#define GPIOC_CRH(MASK, MODE) MODIFY_REG( GPIOC->CRH, MASK, MODE );
#define GPIOD_CRH(MASK, MODE) MODIFY_REG( GPIOD->CRH, MASK, MODE );
#define GPIOE_CRH(MASK, MODE) MODIFY_REG( GPIOE->CRH, MASK, MODE );

#define GP_M(PIN)     (15 << (PIN % 8 << 2))
#define GP_IA(PIN)     0
#define GP_IF(PIN)    (4  << (PIN % 8 << 2))
#define GP_IP(PIN)    (8  << (PIN % 8 << 2))
#define GP_PP10(PIN)  (1  << (PIN % 8 << 2))
#define GP_PP2(PIN)   (2  << (PIN % 8 << 2))
#define GP_PP50(PIN)  (3  << (PIN % 8 << 2))
#define GP_OD10(PIN)  (5  << (PIN % 8 << 2))
#define GP_OD2(PIN)   (6  << (PIN % 8 << 2))
#define GP_OD50(PIN)  (7  << (PIN % 8 << 2))
#define GP_APP10(PIN) (9  << (PIN % 8 << 2))
#define GP_APP2(PIN)  (10 << (PIN % 8 << 2))
#define GP_APP50(PIN) (11 << (PIN % 8 << 2))
#define GP_AOD10(PIN) (13 << (PIN % 8 << 2))
#define GP_AOD2(PIN)  (14 << (PIN % 8 << 2))
#define GP_AOD50(PIN) (15 << (PIN % 8 << 2))

#endif //__GPIO_H__
