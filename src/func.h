#ifndef __FUNC_H__
#define __FUNC_H__
#include "stm32f10x.h"

typedef enum {
  DEC  = ((uint8_t) 0x00), // Decimal output string, clearing insignificant zeros
  HEX = ((uint8_t)  0x10), // Hexadecimal output string, clearing insignificant zeros
  HEX1 = ((uint8_t) 0x01), // Hexadecimal output string len=1
  HEX2 = ((uint8_t) 0x02), // Hexadecimal output string len=2
  HEX3 = ((uint8_t) 0x03), // Hexadecimal output string len=3
  HEX4 = ((uint8_t) 0x04), // Hexadecimal output string len=4
  HEX5 = ((uint8_t) 0x05), // Hexadecimal output string len=5
  HEX6 = ((uint8_t) 0x06), // Hexadecimal output string len=6
  HEX7 = ((uint8_t) 0x07), // Hexadecimal output string len=7
  HEX8 = ((uint8_t) 0x08)  // Hexadecimal output string len=8
}itoa_type_TypeDef;

char* itoa(uint32_t num, itoa_type_TypeDef);

#endif //__FUNC_H__
