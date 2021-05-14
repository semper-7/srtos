#include "func.h"

char* itoa(char* bufer, uint32_t num, uint8_t num_dig, uint8_t type) {
  int i = num_dig;
  if (!i) i = 10;
  char *s = bufer + i;
  *(s) = 0;
  do {
    s--;
    if(!type) {
      *(s) = (uint8_t)(num%10 + 0x30);
      num /= 10;
    } else {
      int n=(num&0xf)+0x30;
      if(n>0x39) n+=7; 
      num>>=4;
      *(s)=n;
    }
    i--;
  } while(i && (num_dig || num) && s!=bufer);
  return s;
}
