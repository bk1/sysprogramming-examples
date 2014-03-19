/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/*
 * test the sizes and value ranges for some common types
 */

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <complex.h>

int main(int argc, char *argv[]) {

  char nbytes[1024];
  char pbytes[1024];
  int i;
  for (i = 0; i < 1024; i++) {
    nbytes[i] = (char) 0xff;
    pbytes[i] = (char) 0x7f;
  }

  char x1 = (char) *((char *) nbytes);
  char y1 = (char) *((char *) pbytes);
  printf("char: %3d %lld %llu %llf >0? %d\n", sizeof(char), (long long) x1, (unsigned long long) x1, (long double) x1, x1 > 0);
  printf("char: %3d %lld %llu %llf >0? %d\n", sizeof(char), (long long) y1, (unsigned long long) y1, (long double) y1, y1 > 0);
  signed char x2 = (signed char) *((signed char *) nbytes);
  signed char y2 = (signed char) *((signed char *) pbytes);
  printf("signed char: %3d %lld %llu %llf >0? %d\n", sizeof(signed char), (long long) x2, (unsigned long long) x2, (long double) x2, x2 > 0);
  printf("signed char: %3d %lld %llu %llf >0? %d\n", sizeof(signed char), (long long) y2, (unsigned long long) y2, (long double) y2, y2 > 0);
  unsigned char x3 = (unsigned char) *((unsigned char *) nbytes);
  unsigned char y3 = (unsigned char) *((unsigned char *) pbytes);
  printf("unsigned char: %3d %lld %llu %llf >0? %d\n", sizeof(unsigned char), (long long) x3, (unsigned long long) x3, (long double) x3, x3 > 0);
  printf("unsigned char: %3d %lld %llu %llf >0? %d\n", sizeof(unsigned char), (long long) y3, (unsigned long long) y3, (long double) y3, y3 > 0);
  short x4 = (short) *((short *) nbytes);
  short y4 = (short) *((short *) pbytes);
  printf("short: %3d %lld %llu %llf >0? %d\n", sizeof(short), (long long) x4, (unsigned long long) x4, (long double) x4, x4 > 0);
  printf("short: %3d %lld %llu %llf >0? %d\n", sizeof(short), (long long) y4, (unsigned long long) y4, (long double) y4, y4 > 0);
  signed short x5 = (signed short) *((signed short *) nbytes);
  signed short y5 = (signed short) *((signed short *) pbytes);
  printf("signed short: %3d %lld %llu %llf >0? %d\n", sizeof(signed short), (long long) x5, (unsigned long long) x5, (long double) x5, x5 > 0);
  printf("signed short: %3d %lld %llu %llf >0? %d\n", sizeof(signed short), (long long) y5, (unsigned long long) y5, (long double) y5, y5 > 0);
  unsigned short x6 = (unsigned short) *((unsigned short *) nbytes);
  unsigned short y6 = (unsigned short) *((unsigned short *) pbytes);
  printf("unsigned short: %3d %lld %llu %llf >0? %d\n", sizeof(unsigned short), (long long) x6, (unsigned long long) x6, (long double) x6, x6 > 0);
  printf("unsigned short: %3d %lld %llu %llf >0? %d\n", sizeof(unsigned short), (long long) y6, (unsigned long long) y6, (long double) y6, y6 > 0);
  int x7 = (int) *((int *) nbytes);
  int y7 = (int) *((int *) pbytes);
  printf("int: %3d %lld %llu %llf >0? %d\n", sizeof(int), (long long) x7, (unsigned long long) x7, (long double) x7, x7 > 0);
  printf("int: %3d %lld %llu %llf >0? %d\n", sizeof(int), (long long) y7, (unsigned long long) y7, (long double) y7, y7 > 0);
  signed int x8 = (signed int) *((signed int *) nbytes);
  signed int y8 = (signed int) *((signed int *) pbytes);
  printf("signed int: %3d %lld %llu %llf >0? %d\n", sizeof(signed int), (long long) x8, (unsigned long long) x8, (long double) x8, x8 > 0);
  printf("signed int: %3d %lld %llu %llf >0? %d\n", sizeof(signed int), (long long) y8, (unsigned long long) y8, (long double) y8, y8 > 0);
  unsigned int x9 = (unsigned int) *((unsigned int *) nbytes);
  unsigned int y9 = (unsigned int) *((unsigned int *) pbytes);
  printf("unsigned int: %3d %lld %llu %llf >0? %d\n", sizeof(unsigned int), (long long) x9, (unsigned long long) x9, (long double) x9, x9 > 0);
  printf("unsigned int: %3d %lld %llu %llf >0? %d\n", sizeof(unsigned int), (long long) y9, (unsigned long long) y9, (long double) y9, y9 > 0);
  long x10 = (long) *((long *) nbytes);
  long y10 = (long) *((long *) pbytes);
  printf("long: %3d %lld %llu %llf >0? %d\n", sizeof(long), (long long) x10, (unsigned long long) x10, (long double) x10, x10 > 0);
  printf("long: %3d %lld %llu %llf >0? %d\n", sizeof(long), (long long) y10, (unsigned long long) y10, (long double) y10, y10 > 0);
  signed long x11 = (signed long) *((signed long *) nbytes);
  signed long y11 = (signed long) *((signed long *) pbytes);
  printf("signed long: %3d %lld %llu %llf >0? %d\n", sizeof(signed long), (long long) x11, (unsigned long long) x11, (long double) x11, x11 > 0);
  printf("signed long: %3d %lld %llu %llf >0? %d\n", sizeof(signed long), (long long) y11, (unsigned long long) y11, (long double) y11, y11 > 0);
  unsigned long x12 = (unsigned long) *((unsigned long *) nbytes);
  unsigned long y12 = (unsigned long) *((unsigned long *) pbytes);
  printf("unsigned long: %3d %lld %llu %llf >0? %d\n", sizeof(unsigned long), (long long) x12, (unsigned long long) x12, (long double) x12, x12 > 0);
  printf("unsigned long: %3d %lld %llu %llf >0? %d\n", sizeof(unsigned long), (long long) y12, (unsigned long long) y12, (long double) y12, y12 > 0);
  long long x13 = (long long) *((long long *) nbytes);
  long long y13 = (long long) *((long long *) pbytes);
  printf("long long: %3d %lld %llu %llf >0? %d\n", sizeof(long long), (long long) x13, (unsigned long long) x13, (long double) x13, x13 > 0);
  printf("long long: %3d %lld %llu %llf >0? %d\n", sizeof(long long), (long long) y13, (unsigned long long) y13, (long double) y13, y13 > 0);
  signed long long x14 = (signed long long) *((signed long long *) nbytes);
  signed long long y14 = (signed long long) *((signed long long *) pbytes);
  printf("signed long long: %3d %lld %llu %llf >0? %d\n", sizeof(signed long long), (long long) x14, (unsigned long long) x14, (long double) x14, x14 > 0);
  printf("signed long long: %3d %lld %llu %llf >0? %d\n", sizeof(signed long long), (long long) y14, (unsigned long long) y14, (long double) y14, y14 > 0);
  unsigned long long x15 = (unsigned long long) *((unsigned long long *) nbytes);
  unsigned long long y15 = (unsigned long long) *((unsigned long long *) pbytes);
  printf("unsigned long long: %3d %lld %llu %llf >0? %d\n", sizeof(unsigned long long), (long long) x15, (unsigned long long) x15, (long double) x15, x15 > 0);
  printf("unsigned long long: %3d %lld %llu %llf >0? %d\n", sizeof(unsigned long long), (long long) y15, (unsigned long long) y15, (long double) y15, y15 > 0);
  ssize_t x16 = (ssize_t) *((ssize_t *) nbytes);
  ssize_t y16 = (ssize_t) *((ssize_t *) pbytes);
  printf("ssize_t: %3d %lld %llu %llf >0? %d\n", sizeof(ssize_t), (long long) x16, (unsigned long long) x16, (long double) x16, x16 > 0);
  printf("ssize_t: %3d %lld %llu %llf >0? %d\n", sizeof(ssize_t), (long long) y16, (unsigned long long) y16, (long double) y16, y16 > 0);
  size_t x17 = (size_t) *((size_t *) nbytes);
  size_t y17 = (size_t) *((size_t *) pbytes);
  printf("size_t: %3d %lld %llu %llf >0? %d\n", sizeof(size_t), (long long) x17, (unsigned long long) x17, (long double) x17, x17 > 0);
  printf("size_t: %3d %lld %llu %llf >0? %d\n", sizeof(size_t), (long long) y17, (unsigned long long) y17, (long double) y17, y17 > 0);
  off_t x18 = (off_t) *((off_t *) nbytes);
  off_t y18 = (off_t) *((off_t *) pbytes);
  printf("off_t: %3d %lld %llu %llf >0? %d\n", sizeof(off_t), (long long) x18, (unsigned long long) x18, (long double) x18, x18 > 0);
  printf("off_t: %3d %lld %llu %llf >0? %d\n", sizeof(off_t), (long long) y18, (unsigned long long) y18, (long double) y18, y18 > 0);
  time_t x19 = (time_t) *((time_t *) nbytes);
  time_t y19 = (time_t) *((time_t *) pbytes);
  printf("time_t: %3d %lld %llu %llf >0? %d\n", sizeof(time_t), (long long) x19, (unsigned long long) x19, (long double) x19, x19 > 0);
  printf("time_t: %3d %lld %llu %llf >0? %d\n", sizeof(time_t), (long long) y19, (unsigned long long) y19, (long double) y19, y19 > 0);
  pid_t x20 = (pid_t) *((pid_t *) nbytes);
  pid_t y20 = (pid_t) *((pid_t *) pbytes);
  printf("pid_t: %3d %lld %llu %llf >0? %d\n", sizeof(pid_t), (long long) x20, (unsigned long long) x20, (long double) x20, x20 > 0);
  printf("pid_t: %3d %lld %llu %llf >0? %d\n", sizeof(pid_t), (long long) y20, (unsigned long long) y20, (long double) y20, y20 > 0);
  pthread_t x21 = (pthread_t) *((pthread_t *) nbytes);
  pthread_t y21 = (pthread_t) *((pthread_t *) pbytes);
  printf("pthread_t: %3d %lld %llu %llf >0? %d\n", sizeof(pthread_t), (long long) x21, (unsigned long long) x21, (long double) x21, x21 > 0);
  printf("pthread_t: %3d %lld %llu %llf >0? %d\n", sizeof(pthread_t), (long long) y21, (unsigned long long) y21, (long double) y21, y21 > 0);
  float x22 = (float) *((float *) nbytes);
  float y22 = (float) *((float *) pbytes);
  printf("float: %3d %lld %llu %llf >0? %d\n", sizeof(float), (long long) x22, (unsigned long long) x22, (long double) x22, x22 > 0);
  printf("float: %3d %lld %llu %llf >0? %d\n", sizeof(float), (long long) y22, (unsigned long long) y22, (long double) y22, y22 > 0);
  double x23 = (double) *((double *) nbytes);
  double y23 = (double) *((double *) pbytes);
  printf("double: %3d %lld %llu %llf >0? %d\n", sizeof(double), (long long) x23, (unsigned long long) x23, (long double) x23, x23 > 0);
  printf("double: %3d %lld %llu %llf >0? %d\n", sizeof(double), (long long) y23, (unsigned long long) y23, (long double) y23, y23 > 0);
  long double x24 = (long double) *((long double *) nbytes);
  long double y24 = (long double) *((long double *) pbytes);
  printf("long double: %3d %lld %llu %llf >0? %d\n", sizeof(long double), (long long) x24, (unsigned long long) x24, (long double) x24, x24 > 0);
  printf("long double: %3d %lld %llu %llf >0? %d\n", sizeof(long double), (long long) y24, (unsigned long long) y24, (long double) y24, y24 > 0);
  double complex x25 = (double complex) *((double complex *) nbytes);
  double complex y25 = (double complex) *((double complex *) nbytes);
  printf("double complex: %3d %lld %llu %llf >0? %d\n", sizeof(double complex), (long long) x25, (unsigned long long) x25, (long double) x25, creal(x25) > 0);
  printf("double complex: %3d %lld %llu %llf >0? %d\n", sizeof(double complex), (long long) y25, (unsigned long long) y25, (long double) y25, creal(y25) > 0);
  float complex x26 = (float complex) *((float complex *) nbytes);
  float complex y26 = (float complex) *((float complex *) pbytes);
  printf("float complex: %3d %lld %llu %llf >0? %d\n", sizeof(float complex), (long long) x26, (unsigned long long) x26, (long double) x26, creal(x26) > 0);
  printf("float complex: %3d %lld %llu %llf >0? %d\n", sizeof(float complex), (long long) y26, (unsigned long long) y26, (long double) y26, creal(y26) > 0);
  int complex x27 = (int complex) *((int complex *) nbytes);
  int complex y27 = (int complex) *((int complex *) pbytes);
  printf("int complex: %3d %lld %llu %llf >0? %d\n", sizeof(int complex), (long long) x27, (unsigned long long) x27, (long double) x27, creal(x27) > 0);
  printf("int complex: %3d %lld %llu %llf >0? %d\n", sizeof(int complex), (long long) y27, (unsigned long long) y27, (long double) y27, creal(y27) > 0);
}
