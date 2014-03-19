#include <complex.h>
#include <stdio.h>

int main() {
  double complex x = 1.0 + 3*I;
  double complex y = 3.0 + 4.0*I;
  double complex z = x+y;
  double complex t = x*y;
  printf("z=%lf+%lfi t=%lf+%lfi\n", creal(z), cimag(z), creal(t), cimag(t));
  
}
