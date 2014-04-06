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

#define SIZE 1024
#define THREAD_COUNT 10

char buff[SIZE];

int done = FALSE;

void *thread_run(void *ptr) {
  char *name = ptr;
  int retcode;
  printf("%s started\n", name);
  sleep(1);
  while (! done) {
    // printf("--> %s: locking\n", name);
    retcode = pthread_mutex_trylock(&mutex1);
    // printf("--> %s: locked\n", name);
    if (retcode == EBUSY) {
      sleep(1);
      continue;
    }
    handle_thread_error(retcode, "pthread_mutex_trylock", THREAD_EXIT);
    char *content = fgets(buff, SIZE, stdin);
    if (content == NULL) {
      retcode = pthread_mutex_unlock(&mutex1);
      printf("--> %s: DONE (null)\n", name);
      fflush(stdout);
      done = TRUE;
      continue;
    }
    char *my_copy = strdup(content);
    if (strlen(my_copy) == 0) {
      retcode = pthread_mutex_unlock(&mutex1);
      printf("--> %s: DONE (empty)\n", name);
      fflush(stdout);
      done = TRUE;
      continue;
    } else {
      sleep(1);
      printf("%s: %s", name, my_copy);
      fflush(stdout);
      retcode = pthread_mutex_unlock(&mutex1);
    }
    // printf("--> %s: sleeping\n", name);
    sleep(1);
    // printf("--> %s: slept\n", name);
  }
  return (void *) NULL;
}

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s -d\n\texample of using nonblocking mutex operation\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  pthread_t thread[THREAD_COUNT];
  int retcode;
  retcode = pthread_mutex_init(&mutex1, NULL);
  handle_thread_error(retcode, "pthread_mutex_init", PROCESS_EXIT);

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
  for (i = 0; i < THREAD_COUNT; i++) {
    printf("main: joining thread %d\n", i);
    retcode = pthread_join(thread[i], NULL);
    handle_thread_error(retcode, "pthread_join", PROCESS_EXIT);
    printf("main: joined thread %d\n", i);
  }
  printf("DONE\n");
  exit(0);
}
