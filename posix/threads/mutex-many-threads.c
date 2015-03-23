/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* use mutex for IPC, but do preparation separately */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#include <itskylib.h>

const size_t N_THREADS = 20;

pthread_mutex_t mutex;

struct timespec timeout;

int use_timeout;

void *run(void *arg) {
  int *argi = (int *) arg;
  int my_tidx = *argi;
  
  int retcode;
  printf("in child %d: sleeping\n", my_tidx);
  sleep(2);
  if (use_timeout) {
    retcode = pthread_mutex_timedlock(&mutex, &timeout);
  } else {
    retcode = pthread_mutex_lock(&mutex);
  }
  handle_thread_error(retcode, "child failed (timed)lock", PROCESS_EXIT);
  printf("child %d got mutex\n", my_tidx);
  sleep(5);
  printf("child %d releases mutex\n", my_tidx);
  pthread_mutex_unlock(&mutex);
  printf("child %d released mutex\n", my_tidx);
  sleep(10);
  printf("child %d exiting\n", my_tidx);
  return NULL;
}

void usage(char *argv0, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n%s\n lock mutexes without lock\n\n%s -t <number>\n lock mutexes with timout after given number of seconds\n", argv0, argv0);
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode;

  use_timeout = (argc >= 2 && strcmp(argv[1], "-t") == 0);
  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  timeout.tv_sec  = (time_t) 200;
  timeout.tv_nsec = (long) 0;
  if (use_timeout && argc >= 3) {
    int t;
    sscanf(argv[2], "%d", &t);
    timeout.tv_sec = (time_t) t;
  }
  printf("timout(%ld sec %ld msec)\n", (long) timeout.tv_sec, (long) timeout.tv_nsec);
  
  pthread_t thread[N_THREADS];
  int tidx[N_THREADS];
  for (size_t i = 0; i < N_THREADS; i++) {
    tidx[i] = (int) i;
    retcode = pthread_create(thread + i, NULL, run, (void *) (tidx + i));
    handle_thread_error(retcode, "creating thread failed", PROCESS_EXIT);
  }
  printf("in parent: setting up\n");

  pthread_mutex_init(&mutex, NULL);
  sleep(2);
  printf("in parent: getting mutex\n");
  if (use_timeout) {
    retcode = pthread_mutex_timedlock(&mutex, &timeout);
  } else {
    retcode = pthread_mutex_lock(&mutex);
  }
  handle_thread_error(retcode, "parent failed (timed)lock", PROCESS_EXIT);
  printf("parent got mutex\n");
  sleep(5);
  printf("parent releases mutex\n");
  pthread_mutex_unlock(&mutex);
  printf("parent released mutex\n");
  printf("parent waiting for child to terminate\n");
  for (size_t i = 0; i < N_THREADS; i++) {
    retcode = pthread_join(thread[i], NULL);
    handle_thread_error(retcode, "join failed", PROCESS_EXIT);
    printf("joined thread %d\n", (int) i);
  }
  pthread_mutex_destroy(&mutex);
  printf("done\n");
  exit(0);
}
