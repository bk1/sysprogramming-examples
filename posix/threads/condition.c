/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* example program for using conditions */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include <itskylib.h>

#define THREAD_COUNT 10

pthread_cond_t  condition_1;
pthread_cond_t  condition_m;

void *thread_run(void *ptr) {
  int retcode;
  char *name = (char *)ptr;
  printf("%s: running\n", name);
  pthread_mutex_t local_mutex;
  retcode = pthread_mutex_init(&local_mutex, NULL);
  handle_thread_error(retcode, "pthread_mutex_init", THREAD_EXIT);
  retcode = pthread_mutex_lock(&local_mutex);
  handle_thread_error(retcode, "pthread_mutex_lock", THREAD_EXIT);
  printf("%s: waiting for condition_1\n", name);
  retcode = pthread_cond_wait(&condition_1, &local_mutex);
  handle_thread_error(retcode, "pthread_cond_wait (1)", THREAD_EXIT);
  printf("%s: condition_1 fullfilled\n", name);
  printf("%s: waiting for condition_m\n", name);
  struct timespec f10sec = get_future(10, 0);
  retcode = pthread_cond_timedwait(&condition_m, &local_mutex, &f10sec);
  handle_thread_error(retcode, "pthread_cond_timedwait (m)", NO_EXIT);
  if (retcode == 0) {
    printf("%s: condition_m fullfilled\n", name);
  }
  retcode = pthread_mutex_unlock(&local_mutex);
  handle_thread_error(retcode, "pthread_mutex_unlock", NO_EXIT);
  retcode = pthread_mutex_destroy(&local_mutex);
  handle_thread_error(retcode, "pthread_mutex_destroy", NO_EXIT);
  return (void *) NULL;
}


void usage(char *argv0, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n");
  printf("%s\n\tcrreates %d thread, which wait on condition.  Sends condition-signal to one, then to all the others.\n\n", argv0, THREAD_COUNT);
  exit(1);
}

int main(int argc, char *argv[]) {

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  pthread_t thread[THREAD_COUNT];
  int retcode;
  retcode = pthread_cond_init(&condition_1, NULL);
  handle_thread_error(retcode, "pthread_cond_init (1)", PROCESS_EXIT);
  retcode = pthread_cond_init(&condition_m, NULL);
  handle_thread_error(retcode, "pthread_cond_init (m)", PROCESS_EXIT);

  int i;
  for (i = 0; i < THREAD_COUNT; i++) {
    char raw_name[20];
    sprintf(raw_name, "thread %3d", i);
    char *name = strdup(raw_name);
    printf("main: starting %s\n", name);
    retcode = pthread_create(&(thread[i]), NULL, thread_run, name);
    handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);
    printf("main: started %s\n", name);
  }
  sleep(5);
  for (i = 0; i < THREAD_COUNT; i++) {
    printf("main sending signal\n");
    retcode = pthread_cond_signal(&condition_1);
    handle_thread_error(retcode, "pthread_cond_signal (1)", PROCESS_EXIT);
    sleep(1);
  }
  sleep(5);
  printf("main sending broadcast\n");
  retcode = pthread_cond_broadcast(&condition_m);
  handle_thread_error(retcode, "pthread_cond_signal (m)", PROCESS_EXIT);
  sleep(5);
  printf("main collecting threads\n");
  for (i = 0; i < THREAD_COUNT; i++) {
    printf("main: joining thread %d\n", i);
    retcode = pthread_join(thread[i], NULL);
    handle_thread_error(retcode, "pthread_join", PROCESS_EXIT);
    printf("main: joined thread %d\n", i);
  }
  printf("DONE\n");
  exit(0);
}
