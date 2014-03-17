#include <stdio.h>
#include <pthread.h>

int main(int argc, char *argv[]) {
  printf("main stared\n");
  pthread_exit(NULL);
  printf("after exit");
  return 0;
}
