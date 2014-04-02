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

#define SIZE 1024
#define THREAD_COUNT 10

struct thread_arg {
  struct string_array arr;
  int thread_idx;
};

enum mt_sort_type { MT_HEAP_SORT, MT_TERNARY_HEAP_SORT, MT_QUICK_SORT, MT_FLASH_SORT, MT_FLASH_SORT_BIN, MT_INSERTION_SORT, MT_MERGE_SORT };

pthread_barrier_t start_barrier;
pthread_barrier_t barrier;

pthread_mutex_t output_mutex;

int thread_count;

struct thread_arg *segments;

enum mt_sort_type selected_mt_sort_type;

int use_psort = FALSE;

int psort_internal_threads = 0;

void *thread_run(void *ptr) {
  int retcode;
  struct thread_arg *arg = (struct thread_arg *) ptr;
  char **strings = arg->arr.strings;
  int len = arg->arr.len;
  int idx = arg->thread_idx;
  
  if (use_psort) {
    switch (selected_mt_sort_type) {
    case MT_HEAP_SORT:
      parallel_hsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL, psort_internal_threads);
      break;
    case MT_TERNARY_HEAP_SORT:
      parallel_ternary_hsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL, psort_internal_threads);
      break;
    case MT_QUICK_SORT:
      parallel_qsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL, psort_internal_threads);
      break;
    case MT_FLASH_SORT:
      parallel_fsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL, metric_str_full, (void *) NULL, psort_internal_threads);
      break;
    case MT_FLASH_SORT_BIN:
      parallel_fsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL, metric_binary_printable_pref, (void *) NULL, psort_internal_threads);
      break;
    case MT_INSERTION_SORT:
      parallel_isort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL, psort_internal_threads);
      break;
    case MT_MERGE_SORT:
      parallel_msort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL, psort_internal_threads);
      break;
    default:
      /* should *never* happen: */
      handle_error_myerrno(-1, -1, "wrong mt_sort_type", PROCESS_EXIT);
    }
  } else {
    switch (selected_mt_sort_type) {
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
    case MT_FLASH_SORT_BIN:
      fsort_r(strings, len, sizeof(char_ptr), compare_str_full, (void *) NULL, metric_binary_printable_pref, (void *) NULL);
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

  for (int step = 1; step < thread_count; step *= 2) {
    retcode = pthread_barrier_wait(& barrier);
    if (retcode == PTHREAD_BARRIER_SERIAL_THREAD) {
      pthread_mutex_lock(&output_mutex);
      /* printf("thread %ld (%d) is PTHREAD_BARRIER_SERIAL_THREAD=%d\n", (long) id, tid, retcode); */
      pthread_mutex_unlock(&output_mutex);
    } else {
      handle_thread_error(retcode, "pthread_barrier_wait", THREAD_EXIT);
    }

    if (idx % (2*step) == 0) {
      int other_idx = idx + step;
      if (other_idx < thread_count) {
        int i = 0;
        int j = 0;
        int k = 0;
        int m = segments[idx].arr.len;
        int n = segments[other_idx].arr.len;
        int total_len = m + n;
        char_ptr *strings = malloc(total_len * sizeof(char_ptr));
        char_ptr *left = segments[idx].arr.strings;
        char_ptr *right = segments[other_idx].arr.strings;
        while (i+j < total_len) {
          if (i >= m) {
            while (j < n) {
              strings[k++] = right[j++];
            }
          } else if (j >= n) {
            while (i < m) {
              strings[k++] = left[i++];
            }
          } else {
            if (strcmp(left[i], right[j]) <= 0) {
              strings[k++] = left[i++];
            } else {
              strings[k++] = right[j++];
            }
          }
        }
        segments[idx].arr.len = total_len;
        segments[idx].arr.strings = strings;
        segments[other_idx].arr.len = 0;
        segments[other_idx].arr.strings = NULL;
      }
    }
  }
  return (void *) NULL;
}

void usage(char *argv0, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n");
  printf("%s -f n\n\tsorts stdin using flashsort in n threads.\n\n", argv0);
  printf("%s -b n\n\tsorts stdin using flashsort with optimized metric function for binaries in n threads.\n\n", argv0);
  printf("%s -h n\n\tsorts stdin using heapsort in n threads.\n\n", argv0);
  printf("%s -t n\n\tsorts stdin using ternary heapsort in n threads.\n\n", argv0);
  printf("%s -q n\n\tsorts stdin using quicksort in n threads.\n\n", argv0);
  printf("%s -i n\n\tsorts stdin using insertionsort in n threads.\n\n", argv0);
  printf("%s -m n\n\tsorts stdin using mergesort in n threads.\n\n", argv0);
  printf("%s -p m -f n\n\tsorts stdin running psort with m flashsort-threads in n threads.\n\n", argv0);
  printf("%s -p m -b n\n\tsorts stdin running psort with m flashsort-threads with optimized metric function for binaries in n threads.\n\n", argv0);
  printf("%s -p m -h n\n\tsort-threadss stdin running psort with m heapsort in n threads.\n\n", argv0);
  printf("%s -p m -t n\n\tsort-threadss stdin running psort with m ternary heapsort in n threads.\n\n", argv0);
  printf("%s -p m -q n\n\tsort-threadss stdin running psort with m quicksort-threads in n threads.\n\n", argv0);
  printf("%s -p m -i n\n\tsort-threadss stdin running psort with m insertionsort-threads in n threads.\n\n", argv0);
  printf("%s -p m -m n\n\tsort-threadss stdin running psort with m mergesort-threads in n threads.\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {
  int retcode;
  pthread_t *thread;

  char *argv0 = argv[0];
  if (is_help_requested(argc, argv)) {
    usage(argv0, "");
  }

  if (argc != 3 && argc != 5) {
    printf("found %d arguments\n", argc - 1);
    usage(argv0, "wrong number of arguments");
  }

  int opt_idx = 1;
  int n_idx = 2;
  use_psort = FALSE;

  if (argc == 5) {
    if (strcmp(argv[1], "-p") != 0) {
      usage(argv0, "wrong first option");
    }
    psort_internal_threads = atoi(argv[2]);
    opt_idx+=2;
    n_idx+=2;
    use_psort = TRUE;
  }

  /* TODO consider using getopt instead!! */
  char *argv_opt = argv[opt_idx];
  if (strlen(argv_opt) != 2 || argv_opt[0] != '-') {
    usage(argv0, "wrong option");
  }
  char opt_char = argv_opt[1];
  switch (opt_char) {
  case 'h' :
    selected_mt_sort_type = MT_HEAP_SORT;
    break;
  case 't' :
    selected_mt_sort_type = MT_TERNARY_HEAP_SORT;
    break;
  case 'f' :
    selected_mt_sort_type = MT_FLASH_SORT;
    break;
  case 'b' :
    selected_mt_sort_type = MT_FLASH_SORT_BIN;
    break;
  case 'q' :
    selected_mt_sort_type = MT_QUICK_SORT;
    break;
  case 'i' :
    selected_mt_sort_type = MT_INSERTION_SORT;
    break;
  case 'm' :
    selected_mt_sort_type = MT_MERGE_SORT;
    break;
  default:
    usage(argv0, "wrong option: only -q and -h supported");
    break;
  }

  sscanf(argv[2], "%d", &thread_count);
  if (thread_count < 1 || thread_count > 1024) {
    printf("running with %d threads\n", thread_count);
    usage(argv[0], "wrong number of threads");
  }


  struct string_array content = read_to_array(STDIN_FILENO);
  int len_per_thread = content.len / thread_count;

  thread = (pthread_t *) malloc(thread_count * sizeof(pthread_t));
  segments = (struct thread_arg *) malloc(thread_count * sizeof(struct thread_arg));

  retcode = pthread_barrier_init(&barrier, NULL, thread_count);
  handle_thread_error(retcode, "pthread_barrier_init", PROCESS_EXIT);
  retcode = pthread_mutex_init(&output_mutex, NULL);
  handle_thread_error(retcode, "pthread_mutex_init", PROCESS_EXIT);

  int rest_len = content.len;
  char **ptr = content.strings;

  for (int i = 0; i < thread_count; i++) {
    segments[i].thread_idx = i;
    segments[i].arr.strings = ptr;
    if (i == thread_count-1) {
      segments[i].arr.len     = rest_len;
    } else {
      segments[i].arr.len = len_per_thread;
    }
    ptr += len_per_thread;
    rest_len -= len_per_thread;

    /* pthread_mutex_lock(&output_mutex); */
    /* printf("main: starting thread %d\n", i); */
    /* pthread_mutex_unlock(&output_mutex); */

    retcode = pthread_create(&(thread[i]), NULL, thread_run, &(segments[i]));
    handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);

    /* pthread_mutex_lock(&output_mutex); */
    /* printf("main: started %d\n", i); */
    /* pthread_mutex_unlock(&output_mutex); */
  }

  for (int i = 0; i < thread_count; i++) {
    /* printf("main: joining thread %d\n", i); */
    retcode = pthread_join(thread[i], NULL);
    handle_thread_error(retcode, "pthread_join", PROCESS_EXIT);
    /* printf("main: joined thread %d\n", i); */
  }
  char_ptr *strings = segments[0].arr.strings;
  int len = segments[0].arr.len;
  for (int i = 0; i < len; i++) {
    /* printf("%5d \"%s\"\n", i, strings[i]); */
    printf("%s\n", strings[i]);
  }
  pthread_barrier_destroy(&barrier);
  // printf("DONE\n");
  exit(0);
}
