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

pthread_once_t  once;

void once_fun() {
  printf("executed only once\n");
}

void *thread_run(void *ptr) {
  int retcode;
  char *name = (char *)ptr;
  printf("%s: running\n", name);
  retcode = pthread_once(&once, once_fun);
  handle_thread_error(retcode, "pthread_mutex_destroy", NO_EXIT);
  printf("%s: returned from once\n", name);
  sleep(1);
  printf("%s: doing its own stuff after once\n", name);
  return (void *) NULL;
}


void usage(char *argv0, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n");
  printf("%s\n\tcrreates %d threads, which run a function once and then do their own stuff.\n\n", argv0, THREAD_COUNT);
  exit(1);
}

int main(int argc, char *argv[]) {

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  pthread_t thread[THREAD_COUNT];
  int retcode;

  once = PTHREAD_ONCE_INIT;

  for (int i = 0; i < THREAD_COUNT; i++) {
    char raw_name[20];
    sprintf(raw_name, "thread %3d", i);
    char *name = strdup(raw_name);
    printf("main: starting %s\n", name);
    retcode = pthread_create(&(thread[i]), NULL, thread_run, name);
    handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);
    retcode = pthread_detach(thread[i]);
    handle_thread_error(retcode, "pthread_detach", PROCESS_EXIT);
    printf("main: started %s\n", name);
  }
  printf("main cwaiting for threads\n");
  sleep(10);
  printf("DONE\n");
  exit(0);
}
