#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void exit_handler() {
  printf("exting\n");
}

int main(int argc, char *argv[]) {
  atexit(exit_handler);
  printf("main stared\n");
  pthread_exit(NULL);
  printf("after exit");
  return 0;
}
