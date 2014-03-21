/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* test reentrance of lock */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <itskylib.h>

pthread_mutex_t mutex1;

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s -d\n mutex_type=PTHREAD_MUTEX_DEFAULT\n\n", argv0);
  printf("%s -n\n mutex_type=PTHREAD_MUTEX_NORMAL\n\n", argv0);
  printf("%s -e\n mutex_type=PTHREAD_MUTEX_ERRORCHECK\n\n", argv0);
  printf("%s -r\n mutex_type=PTHREAD_MUTEX_RECURSIVE\n\n", argv0);
  printf("%s -x\n no attribute used\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {
  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

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
  handle_thread_error(retcode, "pthread_mutex_init", PROCESS_EXIT);
  
  printf("in parent thread: getting mutex1\n");
  retcode = pthread_mutex_lock(&mutex1);
  handle_thread_error(retcode, "lock (1)", PROCESS_EXIT);
  printf("in parent thread: got mutex1\n");
  printf("in parent thread: getting mutex1\n");
  retcode = pthread_mutex_lock(&mutex1);
  handle_thread_error(retcode, "lock (2)", PROCESS_EXIT);
  printf("in parent thread: got mutex1\n");
  sleep(5);
  printf("in parent thread: releasing (1)\n");
  retcode = pthread_mutex_unlock(&mutex1);
  handle_thread_error(retcode, "unlock (1)", PROCESS_EXIT);
  printf("in parent thread: releasing (2)\n");
  retcode = pthread_mutex_unlock(&mutex1);
  handle_thread_error(retcode, "unlock (2)", PROCESS_EXIT);
  printf("in parent thread: done\n");

  retcode = pthread_mutex_destroy(&mutex1);
  handle_thread_error(retcode, "pthread_mutex_destroy", PROCESS_EXIT);
  printf("done\n");
  exit(0);
}
