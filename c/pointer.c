/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/*
 * test the behaviour of pointers
 */

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <complex.h>

int main(int argc, char *argv[]) {

  char nbytes[1024];
  int i;
  for (i = 0; i < 1024; i++) {
    nbytes[i] = (char) i;
  }

  void *x0 = (void *) nbytes;
  void *y0 = x0 + 1;
  void *z0 = y0;
  printf("  void *: d1=%ld d2=%ld\n", (long) ((long) y0 - (long) x0), (long) ((long) z0 - (long) x0));
  char *x1 = (char *) nbytes;
  char *y1 = x1 + 1;
  char *z1 = &(x1[1]);
  printf("  char *: d1=%ld d2=%ld\n", (long) ((long) y1 - (long) x1), (long) ((long) z1 - (long) x1));
  signed char *x2 = (signed char *) nbytes;
  signed char *y2 = x2 + 1;
  signed char *z2 = &(x2[1]);
  printf("  signed char *: d1=%ld d2=%ld\n", (long) ((long) y2 - (long) x2), (long) ((long) z2 - (long) x2));
  unsigned char *x3 = (unsigned char *) nbytes;
  unsigned char *y3 = x3 + 1;
  unsigned char *z3 = &(x3[1]);
  printf("  unsigned char *: d1=%ld d2=%ld\n", (long) ((long) y3 - (long) x3), (long) ((long) z3 - (long) x3));
  short *x4 = (short *) nbytes;
  short *y4 = x4 + 1;
  short *z4 = &(x4[1]);
  printf("  short *: d1=%ld d2=%ld\n", (long) ((long) y4 - (long) x4), (long) ((long) z4 - (long) x4));
  signed short *x5 = (signed short *) nbytes;
  signed short *y5 = x5 + 1;
  signed short *z5 = &(x5[1]);
  printf("  signed short *: d1=%ld d2=%ld\n", (long) ((long) y5 - (long) x5), (long) ((long) z5 - (long) x5));
  unsigned short *x6 = (unsigned short *) nbytes;
  unsigned short *y6 = x6 + 1;
  unsigned short *z6 = &(x6[1]);
  printf("  unsigned short *: d1=%ld d2=%ld\n", (long) ((long) y6 - (long) x6), (long) ((long) z6 - (long) x6));
  int *x7 = (int *) nbytes;
  int *y7 = x7 + 1;
  int *z7 = &(x7[1]);
  printf("  int *: d1=%ld d2=%ld\n", (long) ((long) y7 - (long) x7), (long) ((long) z7 - (long) x7));
  signed int *x8 = (signed int *) nbytes;
  signed int *y8 = x8 + 1;
  signed int *z8 = &(x8[1]);
  printf("  signed int *: d1=%ld d2=%ld\n", (long) ((long) y8 - (long) x8), (long) ((long) z8 - (long) x8));
  unsigned int *x9 = (unsigned int *) nbytes;
  unsigned int *y9 = x9 + 1;
  unsigned int *z9 = &(x9[1]);
  printf("  unsigned int *: d1=%ld d2=%ld\n", (long) ((long) y9 - (long) x9), (long) ((long) z9 - (long) x9));
  long *x10 = (long *) nbytes;
  long *y10 = x10 + 1;
  long *z10 = &(x10[1]);
  printf("  long *: d1=%ld d2=%ld\n", (long) ((long) y10 - (long) x10), (long) ((long) z10 - (long) x10));
  signed long *x11 = (signed long *) nbytes;
  signed long *y11 = x11 + 1;
  signed long *z11 = &(x11[1]);
  printf("  signed long *: d1=%ld d2=%ld\n", (long) ((long) y11 - (long) x11), (long) ((long) z11 - (long) x11));
  unsigned long *x12 = (unsigned long *) nbytes;
  unsigned long *y12 = x12 + 1;
  unsigned long *z12 = &(x12[1]);
  printf("  unsigned long *: d1=%ld d2=%ld\n", (long) ((long) y12 - (long) x12), (long) ((long) z12 - (long) x12));
  long long *x13 = (long long *) nbytes;
  long long *y13 = x13 + 1;
  long long *z13 = &(x13[1]);
  printf("  long long *: d1=%ld d2=%ld\n", (long) ((long) y13 - (long) x13), (long) ((long) z13 - (long) x13));
  signed long long *x14 = (signed long long *) nbytes;
  signed long long *y14 = x14 + 1;
  signed long long *z14 = &(x14[1]);
  printf("  signed long long *: d1=%ld d2=%ld\n", (long) ((long) y14 - (long) x14), (long) ((long) z14 - (long) x14));
  unsigned long long *x15 = (unsigned long long *) nbytes;
  unsigned long long *y15 = x15 + 1;
  unsigned long long *z15 = &(x15[1]);
  printf("  unsigned long long *: d1=%ld d2=%ld\n", (long) ((long) y15 - (long) x15), (long) ((long) z15 - (long) x15));
  ssize_t *x16 = (ssize_t *) nbytes;
  ssize_t *y16 = x16 + 1;
  ssize_t *z16 = &(x16[1]);
  printf("  ssize_t *: d1=%ld d2=%ld\n", (long) ((long) y16 - (long) x16), (long) ((long) z16 - (long) x16));
  size_t *x17 = (size_t *) nbytes;
  size_t *y17 = x17 + 1;
  size_t *z17 = &(x17[1]);
  printf("  size_t *: d1=%ld d2=%ld\n", (long) ((long) y17 - (long) x17), (long) ((long) z17 - (long) x17));
  off_t *x18 = (off_t *) nbytes;
  off_t *y18 = x18 + 1;
  off_t *z18 = &(x18[1]);
  printf("  off_t *: d1=%ld d2=%ld\n", (long) ((long) y18 - (long) x18), (long) ((long) z18 - (long) x18));
  time_t *x19 = (time_t *) nbytes;
  time_t *y19 = x19 + 1;
  time_t *z19 = &(x19[1]);
  printf("  time_t *: d1=%ld d2=%ld\n", (long) ((long) y19 - (long) x19), (long) ((long) z19 - (long) x19));
  pid_t *x20 = (pid_t *) nbytes;
  pid_t *y20 = x20 + 1;
  pid_t *z20 = &(x20[1]);
  printf("  pid_t *: d1=%ld d2=%ld\n", (long) ((long) y20 - (long) x20), (long) ((long) z20 - (long) x20));
  pthread_t *x21 = (pthread_t *) nbytes;
  pthread_t *y21 = x21 + 1;
  pthread_t *z21 = &(x21[1]);
  printf("  pthread_t *: d1=%ld d2=%ld\n", (long) ((long) y21 - (long) x21), (long) ((long) z21 - (long) x21));
  float *x22 = (float *) nbytes;
  float *y22 = x22 + 1;
  float *z22 = &(x22[1]);
  printf("  float *: d1=%ld d2=%ld\n", (long) ((long) y22 - (long) x22), (long) ((long) z22 - (long) x22));
  double *x23 = (double *) nbytes;
  double *y23 = x23 + 1;
  double *z23 = &(x23[1]);
  printf("  double *: d1=%ld d2=%ld\n", (long) ((long) y23 - (long) x23), (long) ((long) z23 - (long) x23));
  long double *x24 = (long double *) nbytes;
  long double *y24 = x24 + 1;
  long double *z24 = &(x24[1]);
  printf("  long double *: d1=%ld d2=%ld\n", (long) ((long) y24 - (long) x24), (long) ((long) z24 - (long) x24));
  double complex *x25 = (double complex *) nbytes;
  double complex *y25 = x25 + 1;
  double complex *z25 = &(x25[1]);
  printf("  double complex *: d1=%ld d2=%ld\n", (long) ((long) y25 - (long) x25), (long) ((long) z25 - (long) x25));
  float complex *x26 = (float complex *) nbytes;
  float complex *y26 = x26 + 1;
  float complex *z26 = &(x26[1]);
  printf("  float complex *: d1=%ld d2=%ld\n", (long) ((long) y26 - (long) x26), (long) ((long) z26 - (long) x26));
  int complex *x27 = (int complex *) nbytes;
  int complex *y27 = x27 + 1;
  int complex *z27 = &(x27[1]);
  printf("  int complex *: d1=%ld d2=%ld\n", (long) ((long) y27 - (long) x27), (long) ((long) z27 - (long) x27));
}
