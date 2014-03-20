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

int main(int argc, char *argv[]) {
  int retcode;

  pthread_mutexattr_t mutex_attr;
  pthread_mutexattr_init(&mutex_attr);
  char mutex_type = 'x'; // default
  if (argc >= 2 && strlen(argv[1]) == 2 && argv[1][0] == '-') {
    mutex_type = argv[1][1];
  }
  switch (mutex_type) {
  case 'd':
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_DEFAULT);
    break;
  case 'n':
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_NORMAL);
    break;
  case 'e':
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
    break;
  case 'r':
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    break;
  case 'x':
    // use NULL;
    break;
  default:
    printf("unknown option, supported: -d -n -e -r -x\n");
    exit(1);
  }
  if (mutex_type == 'x') {
    retcode = pthread_mutex_init(&mutex1, NULL);
  } else {
    retcode = pthread_mutex_init(&mutex1, &mutex_attr);
  }
  handle_thread_error(retcode, "pthread_mutex_init", PROCESS_EXIT);
  
  printf("in parent thread: getting mutex1\n");
  pthread_mutex_lock(&mutex1);
  printf("in parent thread: got mutex1\n");
  printf("in parent thread: getting mutex1\n");
  pthread_mutex_lock(&mutex1);
  printf("in parent thread: got mutex1\n");
  sleep(5);
  printf("in parent thread: releasing\n");
  pthread_mutex_unlock(&mutex1);
  printf("in parent thread: done\n");

  retcode = pthread_mutex_destroy(&mutex1);
  handle_thread_error(retcode, "pthread_mutex_destroy", PROCESS_EXIT);
  printf("done\n");
  exit(0);
}
