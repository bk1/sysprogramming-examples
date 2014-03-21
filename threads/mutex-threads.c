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

pthread_mutex_t mutex;
struct timespec timeout;
int use_timeout;

void *run(void *arg) {
  int retcode;
  printf("in child: sleeping\n");
  sleep(2);
  if (use_timeout) {
    retcode = pthread_mutex_timedlock(&mutex, &timeout);
  } else {
    retcode = pthread_mutex_lock(&mutex);
  }
  handle_thread_error(retcode, "child failed (timed)lock", PROCESS_EXIT);
  printf("child got mutex\n");
  sleep(5);
  printf("child releases mutex\n");
  pthread_mutex_unlock(&mutex);
  printf("child released mutex\n");
  sleep(10);
  printf("child exiting\n");
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

  pthread_t thread;  pthread_create(&thread, NULL, run, NULL);
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
  pthread_join(thread, NULL);
  pthread_mutex_destroy(&mutex);
  printf("done\n");
  exit(0);
}
