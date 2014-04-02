/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <itskylib.h>

struct result {
  char *min;
  char *max;
};

/* thread function */
void *run(void *raw_name) {
  // int retcode;
  // time_t thread_start = time(NULL);
  char *name = (char *) raw_name;

  // time_t open_start = time(NULL);
  int fd = open_retry(name, O_RDONLY, THREAD_EXIT);
  // time_t open_duration = time(NULL) - open_start;

  struct string_array strings = read_to_array(fd);
  close(fd);
  struct result *result = malloc(sizeof(struct result));
  if (strings.len == 0) {
    result->min = "";
    result->max = "";
    return result;
  }

  result->min = strings.strings[0];
  result->max = strings.strings[0];
  for (int i = 0; i < strings.len; i++) {
    char *str = strings.strings[i];
    if (result->min[0] == '\000' || strcmp(result->min, str) > 0) {
      result->min = str;
    }
    if (result->max[0] == '\000' || strcmp(result->max, str) < 0) {
      result->max = str;
    }
  }
  return result;
}

void usage(char *argv0, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage\n\n");
  printf("%s file1 file2 file3 ... filen\n", argv0);
  printf("\n");
  printf("splits file contents into words and finds the minimum and maximum according to usual string compare.\n");
  printf("uses one thread for each file and then merges the results.\n");
  exit(1);
}

int main(int argc, char *argv[]) {

  char *argv0 = argv[0];
  if (is_help_requested(argc, argv)) {
    usage(argv0, "");
  }

  if (argc < 2) {
    usage(argv0, "not enough arguments");
  }

  time_t start_time = time(NULL);
  int retcode = 0;
  int i;

  printf("%d files will be read\n", argc-1);
  fflush(stdout);

  pthread_t *threads = (pthread_t *) malloc((argc-1)*sizeof(pthread_t));
  for (i = 1; i < argc; i++) {
    retcode = pthread_create(&(threads[i-1]), NULL, run, argv[i]);
    handle_thread_error(retcode, "starting thread", PROCESS_EXIT);
  }
  struct result *result = NULL;
  for (i = 0; i < argc-1; i++) {
    struct result *partial_result;
    retcode = pthread_join(threads[i], (void **) &partial_result);
    handle_thread_error(retcode, "joining thread", PROCESS_EXIT);
    if (partial_result == NULL) {
      printf("thread %d for file %s gave no result\n", i, argv[i+1]);
      continue;
    } else if (partial_result == PTHREAD_CANCELED) {
      printf("thread %d for file %s has been cancelled\n", i, argv[i+1]);
      continue;
    }
    if (result == NULL) {
      result = partial_result;
    } else {
      if (strcmp(result->min, partial_result->min) > 0) {
        result->min = partial_result->min;
      }
      if (strcmp(result->max, partial_result->max) < 0) {
        result->max = partial_result->max;
      }
    }
  }

  printf("------------------------------------------------------------\n");
  printf("min=\"%s\"\n", result->min);
  printf("max=\"%s\"\n", result->max);
  printf("------------------------------------------------------------\n");

  time_t total_time = time(NULL) - start_time;
  printf("total %ld sec\n", (long) total_time);
  printf("done\n");
  exit(0);
}
