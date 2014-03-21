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

void *thread_run(void *ptr) {
  
  printf("in child thread: getting mutex2\n");
  pthread_mutex_lock(&mutex2);
  printf("in child thread: got mutex2\n");
  sleep(5);
  printf("in child thread: getting mutex1\n");
  pthread_mutex_lock(&mutex1);
  printf("in child thread: got mutex1\n");
  sleep(5);
  printf("in child thread: releasing\n");
  pthread_mutex_unlock(&mutex1);
  pthread_mutex_unlock(&mutex2);
  printf("in child thread: done\n");

  return (void *) NULL;
}

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s -d\n PTHREAD_MUTEX_DEFAULT\n\n", argv0);
  printf("%s -n\n PTHREAD_MUTEX_NORMAL\n\n", argv0);
  printf("%s -e\n PTHREAD_MUTEX_ERRORCHECK\n\n", argv0);
  printf("%s -r\n PTHREAD_MUTEX_RECURSIVE\n\n", argv0);
  printf("%s -x\n no attribute used\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {
  int retcode;
  pthread_t thread1;

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
    usage(argv[0], "unknown option, supported: -d -n -e -r -x");
  }
  if (mutex_type == 'x') {
    retcode = pthread_mutex_init(&mutex1, NULL);
  } else {
    retcode = pthread_mutex_init(&mutex1, &mutex_attr);
  }
  handle_thread_error(retcode, "pthread_mutex_init (1)", PROCESS_EXIT);

  if (mutex_type == 'x') {
    retcode = pthread_mutex_init(&mutex2, NULL);
  } else {
    retcode = pthread_mutex_init(&mutex2, &mutex_attr);
  }
  handle_thread_error(retcode, "pthread_mutex_init (2)", PROCESS_EXIT);

  retcode = pthread_mutexattr_destroy(&mutex_attr);
  handle_thread_error(retcode, "pthread_mutexattr_destroy", PROCESS_EXIT);

  retcode = pthread_create(&thread1, NULL, thread_run, NULL);
  handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);
  
  printf("in parent thread: getting mutex1\n");
  pthread_mutex_lock(&mutex1);
  printf("in parent thread: got mutex1\n");
  sleep(5);
  printf("in parent thread: getting mutex2\n");
  pthread_mutex_lock(&mutex2);
  printf("in parent thread: got mutex2\n");
  sleep(5);
  printf("in parent thread: releasing\n");
  pthread_mutex_unlock(&mutex2);
  pthread_mutex_unlock(&mutex1);
  printf("in parent thread: done\n");

  retcode = pthread_join( thread1, NULL);
  handle_thread_error(retcode, "pthread_join", PROCESS_EXIT);

  retcode = pthread_mutex_destroy(&mutex1);
  handle_thread_error(retcode, "pthread_mutex_destroy", PROCESS_EXIT);
  retcode = pthread_mutex_destroy(&mutex2);
  handle_thread_error(retcode, "pthread_mutex_destroy", PROCESS_EXIT);
  printf("done\n");
  exit(0);
}
