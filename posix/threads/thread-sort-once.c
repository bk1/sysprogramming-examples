/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* enable qsort_r */
#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include <itskylib.h>
#include <hsort.h>
#include <fsort.h>
#include <isort.h>
#include <ternary-hsort.h>
#include <msort.h>
#include <fsort-metrics.h>
#include <psort.h>

const int LINE_SIZE = 200;
const int THREAD_COUNT = 20;

pthread_once_t once = PTHREAD_ONCE_INIT;

enum mt_sort_type { MT_HEAP_SORT, MT_TERNARY_HEAP_SORT, MT_QUICK_SORT, MT_FLASH_SORT, MT_INSERTION_SORT, MT_MERGE_SORT };

const char *file_name;
enum mt_sort_type selected_sort_type;
struct string_array content;


struct thread_arg {
  int thread_idx;
  pthread_once_t *once;
};

/* needs to be global here for the sake of thread_once */
struct thread_arg *thread_data;

void *thread_run(void *ptr);

void print_sorted_words(struct thread_arg *arg);

void thread_once();

void usage(char *argv0, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n");

  printf("splits the given input file into words, disregarding all characters other than [A-Za-z0-9\240-\377]\n");
  printf("(non-used characters mark word boundaries)\n");
  printf("sorts the words and prints out the sorted words in a 200 character wide output\n");
  printf("words longer than 200 characters exceed the width in a line by themselves\n");
  printf("this is done 20 times in different threads started at random times 0..9 sec delayed after program start\n");
  printf("reading and sorting is only done once by the thread that comes first, output is then rendered and printed by each thread separately\n\n");

  printf("%s -f file \n\tsorts contents of file using flashsort.\n\n", argv0);
  printf("%s -h file \n\tsorts contents of file using heapsort.\n\n", argv0);
  printf("%s -t file \n\tsorts contents of file using ternary heapsort.\n\n", argv0);
  printf("%s -q file \n\tsorts contents of file using quicksort.\n\n", argv0);
  printf("%s -i file \n\tsorts contents of file using insertionsort.\n\n", argv0);
  printf("%s -m file \n\tsorts contents of file using mergesort.\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {
  int retcode;
  pthread_t *thread;

  char *argv0 = argv[0];
  if (argc != 3) {
    printf("found %d arguments\n", argc - 1);
    usage(argv0, "wrong number of arguments");
  }

  int opt_idx = 1;

  /* TODO consider using getopt instead!! */
  char *argv_opt = argv[opt_idx];
  if (strlen(argv_opt) != 2 || argv_opt[0] != '-') {
    usage(argv0, "wrong option");
  }
  char opt_char = argv_opt[1];
  switch (opt_char) {
  case 'h' :
    selected_sort_type = MT_HEAP_SORT;
    break;
  case 't' :
    selected_sort_type = MT_TERNARY_HEAP_SORT;
    break;
  case 'f' :
    selected_sort_type = MT_FLASH_SORT;
    break;
  case 'q' :
    selected_sort_type = MT_QUICK_SORT;
    break;
  case 'i' :
    selected_sort_type = MT_INSERTION_SORT;
    break;
  case 'm' :
    selected_sort_type = MT_MERGE_SORT;
    break;
  default:
    usage(argv0, "wrong option");
    break;
  }

  file_name = argv[2];

  thread = (pthread_t *) malloc(THREAD_COUNT * sizeof(pthread_t));
  thread_data = (struct thread_arg *) malloc(THREAD_COUNT * sizeof(struct thread_arg));

  for (int i = 0; i < THREAD_COUNT; i++) {
    thread_data[i].thread_idx = i;
    thread_data[i].once = &once;
    retcode = pthread_create(&(thread[i]), NULL, thread_run, &(thread_data[i]));
    handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);
  }

  for (int i = 0; i < THREAD_COUNT; i++) {
    /* printf("main: joining thread %d\n", i); */
    retcode = pthread_join(thread[i], NULL);
    handle_thread_error(retcode, "pthread_join", PROCESS_EXIT);
    /* printf("main: joined thread %d\n", i); */
  }
  exit(0);
}

void thread_once() {
  // int retcode;
  int fd = open(file_name, O_RDONLY);
  handle_error(fd, "open", PROCESS_EXIT);
  content = read_to_array(fd);
  close(fd);

  char **strings = content.strings;
  size_t len = content.len;
  switch (selected_sort_type) {
  case MT_HEAP_SORT:
    hsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL);
    break;
  case MT_TERNARY_HEAP_SORT:
    ternary_hsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL);
    break;
  case MT_QUICK_SORT:
    qsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL);
    break;
  case MT_FLASH_SORT:
    fsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL, metric_str_full, (void *) NULL);
    break;
  case MT_INSERTION_SORT:
    isort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL);
    break;
  case MT_MERGE_SORT:
    msort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL);
    break;
  default:
    /* should *never* happen: */
    handle_error_myerrno(-1, -1, "wrong mt_sort_type", PROCESS_EXIT);
  }

}

void print_sorted_words(struct thread_arg *arg) {

  int retcode;
  retcode = pthread_once(arg->once, thread_once);
  handle_thread_error(retcode, "pthread_once", THREAD_EXIT);

  char **strings = content.strings;
  int idx = arg->thread_idx;

  flockfile(stdout);
  printf("\n------------------------------------------------------------\n");
  printf("thread %d\n", idx);
  printf("------------------------------------------------------------\n");

  char line[LINE_SIZE+1];
  char *pos = line;
  size_t used_len = 0;
  for (int i = 0; i < content.len; i++) {
    int len = strlen(strings[i]) + 1; /* allow for space before... */
    if (used_len + len > LINE_SIZE) {
      printf("%s\n", line);
      pos = line;
      used_len = 0;
    }
    if (len > LINE_SIZE+1) {
      printf("%s\n", strings[i]);
    } else {
      sprintf(pos, " %s", strings[i]);
      pos += len;
      used_len += len;
    }
  }
  if (used_len > 0) {
    printf("%s\n", line);
  }

  printf("------------------------------------------------------------\n\n");
  funlockfile(stdout);
}

void *thread_run(void *ptr) {
  int retcode;
  struct thread_arg *arg = (struct thread_arg *) ptr;
  int random_number = rand();
  unsigned int duration = (unsigned int) random_number % 10;
  printf("thread %d sleeping for %d sec\n", arg->thread_idx, duration);
  retcode = sleep(duration);
  handle_error(retcode, "sleep", THREAD_EXIT);
  print_sorted_words(arg);
  return (void *) NULL;
}
