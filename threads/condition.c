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

pthread_mutex_t mutex1;
pthread_cond_t  condition;

void *thread_run(void *ptr) {
  char *name = (char *)ptr;
  printf("%s: running\n", name);
  pthread_mutex_t local_mutex;
  pthread_mutex_init(&local_mutex, NULL);
  pthread_mutex_lock(&local_mutex);
  pthread_cond_wait(&condition, &local_mutex);
  printf("%s: cond fullfilled\n", name);
  pthread_mutex_unlock(&local_mutex);
  pthread_mutex_destroy(&local_mutex);
  return (void *) NULL;
}

int main(int argc, char *argv[]) {
  pthread_t thread[THREAD_COUNT];
  int retcode;
  retcode = pthread_mutex_init(&mutex1, NULL);
  retcode = pthread_cond_init(&condition, NULL);
  handle_thread_error(retcode, "pthread_cond_init", PROCESS_EXIT);

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
  printf("main sending signal\n");
  pthread_cond_signal(&condition);
  sleep(5);
  printf("main sending broadcast\n");
  pthread_cond_broadcast(&condition);
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
