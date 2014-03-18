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

#include <itskylib.h>

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

int main(int argc, char *argv[]) {
  int retcode;
  retcode = pthread_mutex_init(&mutex1, NULL);
  handle_thread_error(retcode, "pthread_mutex_init", PROCESS_EXIT);
  retcode = pthread_mutex_init(&mutex2, NULL);
  handle_thread_error(retcode, "pthread_mutex_init", PROCESS_EXIT);
  
  printf("in parent thread: getting mutex1\n");
  pthread_mutex_lock(&mutex1);
  printf("in parent thread: got mutex1\n");
  sleep(1);
  printf("in parent thread: getting mutex2\n");
  pthread_mutex_lock(&mutex2);
  printf("in parent thread: got mutex2\n");
  sleep(1);
  printf("in parent thread: releasing mutex1\n");
  pthread_mutex_unlock(&mutex1);
  printf("in parent thread: releasing mutex2\n");
  pthread_mutex_unlock(&mutex2);
  printf("in parent thread: done\n");

  retcode = pthread_mutex_destroy(&mutex1);
  handle_thread_error(retcode, "pthread_mutex_destroy", PROCESS_EXIT);
  retcode = pthread_mutex_destroy(&mutex2);
  handle_thread_error(retcode, "pthread_mutex_destroy", PROCESS_EXIT);
  printf("done\n");
  exit(0);
}
