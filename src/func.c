#include "func.h"

char buf_ansi[11];

char* itoa(uint32_t num,uint8_t type) {
  char *s=buf_ansi+10;
  *(s)=0;
  do {
    s--;
    if(--type>128) {
      *(s)=(uint8_t)(num%10 + 0x30);
      num/=10;
    } else {
      int n=(num&0xf)+0x30;
      if(n>0x39) n+=7; 
      num>>=4;
      *(s)=n;
    }
  } while(type && (type<11 || num) && s!=buf_ansi);
  return s;
}
