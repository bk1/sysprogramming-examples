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

int pipes[2];
int message_len;

void *thread_run(void *ptr) {
  char **lines = (char **) ptr;
  int m = atoi(lines[0]);
  int thread_idx = atoi(lines[1]);
  flockfile(stdout);
  for (int i = 0; i < m; i++) {
    printf("thread=%4d / line=%4d: %s\n", thread_idx, i, lines[i]);
  }
  funlockfile(stdout);
  return (void *) NULL;
}

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s n <file\n\tExample of creating n threads and multiline output in each thread, but without mixing.\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  if (argc != 2) {
    usage(argv[0], "wrong number of arguments");
  }

  int n = atoi(argv[1]);
  if (n <= 0) {
    usage(argv[0], "n must be > 0");
  }

  struct string_array strings = read_to_array(STDIN_FILENO);

  size_t rest_len = strings.len;
  size_t rest_cnt = n;
  size_t pos = 0;

  for (int j = 0; j < n; j++) {
    size_t m = rest_len / rest_cnt + 3; // 1 for rounding, 1 for length, 1 for thread_idx
    size_t len = m - 2;
    char **array = (char **) malloc(m * sizeof(char *));
    char mbuf[20];
    char jbuf[20];
    sprintf(mbuf, "%ld", (long) m);
    sprintf(jbuf, "%ld", (long) j);
    array[0] = mbuf;
    array[1] = jbuf;
    for (int i = 2; i < m; i++) {
      array[i] = strings.strings[pos++];
    }
    pthread_t thread;
    pthread_create(&thread, NULL, thread_run, array);
    pthread_detach(thread);
    rest_len -= len;
    rest_cnt--;
  }

  sleep(20);
  exit(0);
}
