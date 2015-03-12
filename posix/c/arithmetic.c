/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>

#include <itskylib.h>

typedef unsigned long long uint64;
typedef signed   long long sint64;

struct signed_result_with_carry {
  sint64 value;
  short carry;
};

struct unsigned_result_with_carry {
  uint64 value;
  short carry;
};

typedef struct unsigned_result_with_carry unsigned_result_with_carry;
typedef struct signed_result_with_carry signed_result_with_carry;

unsigned_result_with_carry add(uint64 x, uint64 y) {
  unsigned_result_with_carry result;
  uint64 z = x + y;
  int cx = (x & 0x8000) != 0;
  int cy = (y & 0x8000) != 0;
  int cz = (z & 0x8000) != 0;
  int cc = (cx & ! cz) | (cy & ! cz) | (cx & cy & cz);
  result.value = z;
  result.carry = cc;
  return result;
}

unsigned_result_with_carry adc(uint64 x, uint64 y, int carry) {
  unsigned_result_with_carry result;
  uint64 z = x + y + carry;
  int cx = (x & 0x8000) != 0;
  int cy = (y & 0x8000) != 0;
  int cz = (z & 0x8000) != 0;
  int cc = (cx & ! cz) | (cy & ! cz) | (cx & cy & cz);
  result.value = z;
  result.carry = cc;
  return result;
}

uint128_t mul(uint64 x, uint64 y) {
  uint128_t result = (uint128_t) x * (uint128_t) y;
  return result;
}

unsigned_result_with_carry adc_s(unsigned_result_with_carry rx, uint64 y) {
  return adc(rx.value, y, rx.carry);
}

int main(int argc, char **argv) {
  for (unsigned short xh = 0; xh <= 1; xh++) {
    for (unsigned short yh = 0; yh <= 1; yh++) {
      for (unsigned short xl = 0; xl <= 0x7fff; xl++) {
        unsigned short x = xh*0x8000 + xl;
        if (xl == 0x0080) {
          xl = 0x7f00;
        }
        for (unsigned short yl = 0; yl <= 0x7fff; yl++) {
          unsigned short y = yh*0x8000 + yl;
          if (yl == 0x0080) {
            yl = 0x7f00;
          }
          for (int carry = 0; carry <= 1; carry++) {
            int xx = x;
            int yy = y;
            int zz = xx + yy + carry;
            int cb = (0x10000 & zz) != 0;
            unsigned short z = x+y+carry;
            int cx = (x & 0x8000) != 0;
            int cy = (y & 0x8000) != 0;
            int cz = (z & 0x8000) != 0;
            int cc = (cx & ! cz) | (cy & ! cz) | (cx & cy & cz);
            // printf("cb=%d cc=%d cx=%d cy=%d cz=%d (x=%d y=%d z=%d xx=%d yy=%d zz=%d)\n", cb, cc, cx, cy, cz, (int) x, (int) y, (int) z, xx, yy, zz);
            printf("cb=%d cc=%d cx=%d cy=%d cz=%d ca=%d\n", cb, cc, cx, cy, cz, carry);
          }
        }
      }
    }
  }
}
