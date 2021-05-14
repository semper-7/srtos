#ifndef __FUNC_H__
#define __FUNC_H__
#include "stm32f10x.h"

typedef enum {
  DEC = ((uint8_t) 0), // Decimal output string
  HEX = ((uint8_t) 1), // Hexadecimal output string
}itoa_type_TypeDef;

char* itoa(char* bufer, uint32_t num, uint8_t num_dig, itoa_type_TypeDef);

#endif //__FUNC_H__
