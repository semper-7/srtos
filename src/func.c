#include "func.h"

char* utoa(uint32_t num, char *buffer, uint8_t num_dig, utoa_base_TypeDef base)
{
  uint32_t quot;
  uint32_t tmp;
  uint8_t rem;
  char* s;
  if (num_dig)
  {
    s = buffer + num_dig;
  }
  else
  {
    s = buffer + 10;
  }
  *s = 0;

  if (!base)
  {
    do
    {
      // "* 0.8"
      quot = num >> 1;
      quot += quot >> 1;
      quot += quot >> 4;
      quot += quot >> 8;
      quot += quot >> 16;
      tmp = quot;
      // "/ 8"
      quot >>= 3;
      rem = (uint8_t)(num - ((quot << 1) + (tmp & ~7ul)));
      if(rem > 9)
      {
        rem -= 10;
        quot++;
      }
      *--s = rem + '0';
      num = quot;
    } while (s != buffer && (num_dig + num));
  }
  else
  {
    do
    {
      rem = num & 0xf;
      if (rem > 9) rem += 7;
      *--s = rem + '0';
      num >>= 4;
    } while (s != buffer && (num_dig + num));
  }
  return s;
}
