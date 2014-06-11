#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int x;


void *thread_run(void *ptr) {
  x++;
  return NULL;
}

int main(int argc, char *argv[]) {
  x = 0;
  pthread_t thread;

  pthread_create(&thread, NULL, thread_run, NULL);

  printf("%d\n", x);
  exit(0);
}
