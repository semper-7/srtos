#include "func.h"
#include "asmfunc.h"

char* itoa(char* bufer, uint32_t num, uint8_t num_dig, uint8_t type) {
  char *s;
  if (num_dig) {
    s = &bufer[num_dig];
  } else {
    s = &bufer[10];
  }
  *(s) = 0;
  if(type) {
    do {
      int n = (num&0xf)+0x30;
      if (n>0x39) n += 7; 
      *(--s) = n;
      num >>= 4;
    } while(s!=bufer && (num_dig || num));
  } else {
    do {
      *(--s) = (uint8_t)(num%10 + 0x30);
      num /= 10;
    } while(s!=bufer && (num_dig || num));
  }
  if (num_dig) {
    return bufer + num_dig;
  } else {
    return __stpcpy(bufer, s);
  }
}
