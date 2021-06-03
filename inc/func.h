#ifndef __FUNC_H__
#define __FUNC_H__
#include "stm32f103xb.h"

typedef enum {
  DEC = ((uint8_t) 0), // Decimal output string
  HEX = ((uint8_t) 1), // Hexadecimal output string
} utoa_base_TypeDef;

char* utoa(uint32_t num, char *buffer, uint8_t num_dig, utoa_base_TypeDef base);

#endif //__FUNC_H__
