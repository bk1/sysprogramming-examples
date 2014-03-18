/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define SIZE 1024
#define THREAD_COUNT 10

pthread_barrier_t barrier;

void handle_thread_error(int retcode) {
  if (retcode < 0) {
    printf("thread error %d\n", retcode);
    exit(1);
  }
}

void *thread_run(void *ptr) {
  long *val_ptr = (long *) ptr;
  long val = *val_ptr;
  long i;
  
  pthread_t id = pthread_self();

  for (i = 0; i < 100; i++) {
    long x = val + i;
    long y = x*x;
    printf("%20ld (%3ld)  %4ld^2=%8ld\n", (long) id, val, x, y);
    fflush(stdout);
    if (i % 10 == 9) {
      printf("%20ld (%3ld) waiting for others\n", (long) id, val);
      fflush(stdout);
      pthread_barrier_wait(& barrier);
      printf("%20ld (%3ld) continuing\n", (long) id, val);
      fflush(stdout);
    }
  }
  printf("%20ld (%3ld) done\n", (long) id, val);
  fflush(stdout);
  return (void *) NULL;
}

int main(int argc, char *argv[]) {
  pthread_t thread[THREAD_COUNT];
  int retcode;
  retcode = pthread_barrier_init(&barrier, NULL, THREAD_COUNT);
  handle_thread_error(retcode);

  int i;
  for (i = 0; i < THREAD_COUNT; i++) {
    long val = i * 100;
    long *val_ptr = malloc(sizeof(long));
    *val_ptr = val;
    printf("main: starting %d\n", i);
    retcode = pthread_create(&(thread[i]), NULL, thread_run, val_ptr);
    handle_thread_error(retcode);
    printf("main: started %d\n", i);
  }
  for (i = 0; i < THREAD_COUNT; i++) {
    printf("main: joining thread %d\n", i);
    retcode = pthread_join(thread[i], NULL);
    handle_thread_error(retcode);
    printf("main: joined thread %d\n", i);
  }
  printf("DONE\n");
  exit(0);
}
