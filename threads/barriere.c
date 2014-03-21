/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* simple example program for using barriers */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include <itskylib.h>

#define SIZE 1024
#define THREAD_COUNT 10

pthread_barrier_t start_barrier;
pthread_barrier_t barrier;

int wait_count = 0;

void *thread_run(void *ptr) {
  int retcode;
  long *val_ptr = (long *) ptr;
  long val = *val_ptr;
  long i;
  pthread_t id = pthread_self();

  printf("entered thread %ld (%ld) wait_count=%d\n", (long) id, val, wait_count++);
  
  retcode = pthread_barrier_wait(& start_barrier);
  if (retcode == PTHREAD_BARRIER_SERIAL_THREAD) {
    printf("thread %ld (%ld) is PTHREAD_BARRIER_SERIAL_THREAD=%d\n", (long) id, val, retcode);
  } else {
    handle_thread_error(retcode, "pthread_barrier_wait (start)", THREAD_EXIT);
  }

  printf("activated thread %ld (%ld)\n", (long) id, val);

  for (i = 0; i < 100; i++) {
    long x = val + i;
    long y = x*x;
    printf("%20ld (%3ld)  %4ld^2=%8ld\n", (long) id, val, x, y);
    fflush(stdout);
    if (i % 10 == 9) {
      printf("%20ld (%3ld) waiting for others\n", (long) id, val);
      fflush(stdout);
      retcode = pthread_barrier_wait(& barrier);
      if (retcode == PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("thread %ld (%ld) is PTHREAD_BARRIER_SERIAL_THREAD=%d\n", (long) id, val, retcode);
      } else {
        handle_thread_error(retcode, "pthread_barrier_wait", THREAD_EXIT);
      }

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
  handle_thread_error(retcode, "pthread_barrier_init", PROCESS_EXIT);
  retcode = pthread_barrier_init(&start_barrier, NULL, THREAD_COUNT+1);
  handle_thread_error(retcode, "pthread_barrier_init", PROCESS_EXIT);

  int i;
  for (i = 0; i < THREAD_COUNT; i++) {
    long val = i * 100;
    long *val_ptr = malloc(sizeof(long));
    *val_ptr = val;
    printf("main: starting %d\n", i);
    retcode = pthread_create(&(thread[i]), NULL, thread_run, val_ptr);
    handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);
    printf("main: started %d\n", i);
  }
  
  printf("main: waiting for barrier wait_count=%d\n", wait_count++);
  retcode = pthread_barrier_wait(&start_barrier);
  if (retcode == PTHREAD_BARRIER_SERIAL_THREAD) {
    printf("main is PTHREAD_BARRIER_SERIAL_THREAD=%d\n", retcode);
  } else {
    handle_thread_error(retcode, "main: pthread_barrier_wait (start)", PROCESS_EXIT);
  }

  for (i = 0; i < THREAD_COUNT; i++) {
    printf("main: joining thread %d\n", i);
    retcode = pthread_join(thread[i], NULL);
    handle_thread_error(retcode, "pthread_join", PROCESS_EXIT);
    printf("main: joined thread %d\n", i);
  }
  pthread_barrier_destroy(&barrier);
  pthread_barrier_destroy(&start_barrier);
  printf("DONE\n");
  exit(0);
}
