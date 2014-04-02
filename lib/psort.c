/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* implements parallelized (multithreaded) versions of sort algorithms */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <stdlib.h>

#include <pthread.h>
#include <assert.h>

#include <itskylib.h>
#include <hsort.h>
#include <ternary-hsort.h>
#include <isort.h>
#include <msort.h>
#include <fsort.h>
#include <psort.h>
#include <sortimpl.h>

#include <time.h>

struct sortable_data {
  void  *base;
  size_t nmemb;
  size_t size;
};

/** given as parameter to all threads */
struct psort_thread_data {
  unsigned int thread_count; // number of threads (also referred to as n)
  unsigned int thread_idx; // identifies the thread with an index 0..n-1 (n==thread_count)
  // struct sortable_data own_segment;
  // struct sortable_data all;
  struct sortable_data *segments; // one for each thread
  pthread_barrier_t *barrier_ptr;
  enum sort_type selected_sort_type;
  compare_fun3 compare;
  void *compare_data;
  metric_fun2 metric;
  void *metric_data;
};

void *psort_thread_run(void *ptr) {
  int retcode;
  struct psort_thread_data *data = (struct psort_thread_data *) ptr;
  int idx = data->thread_idx;
  time_t t0 = time(NULL);
  void *base = data->segments[idx].base;
  size_t nmemb = data->segments[idx].nmemb;
  size_t size  = data->segments[idx].size;
  unsigned int thread_count = data->thread_count;
  pthread_barrier_t  *barrier_ptr = data->barrier_ptr;
  compare_fun3       compare      = data->compare;
  void              *compare_data = data->compare_data;
  metric_fun2        metric       = data->metric;
  void              *metric_data  = data->metric_data;
  struct sortable_data *segments  = data->segments;
  // printf("idx=%d base=%p nmemb=%ld size=%ld: sorting\n", idx, base, nmemb, size);

  switch (data->selected_sort_type) {
  case HEAP_SORT:
    hsort_r(base, nmemb, size, compare, compare_data);
    break;
  case TERNARY_HEAP_SORT:
    ternary_hsort_r(base, nmemb, size, compare, compare_data);
    break;
  case QUICK_SORT:
    qsort_r(base, nmemb, size, compare, compare_data);
    break;
  case FLASH_SORT:
    fsort_r(base, nmemb, size, compare, compare_data, metric, metric_data);
    break;
  case INSERTION_SORT:
    isort_r(base, nmemb, size, compare, compare_data);
    break;
  case MERGE_SORT:
    msort_r(base, nmemb, size, compare, compare_data);
    break;
  default:
    /* should *never* happen: */
    handle_error_myerrno(-1, -1, "wrong sort_type", PROCESS_EXIT);
  }

  time_t t1 = time(NULL);
  printf("idx=%d tid=%ld sorted: t=%ld\n", idx, (long) pthread_self(), (long)(t1-t0));

  for (unsigned int step = 1; step < thread_count; step *= 2) {
    // printf("idx=%d waiting for barrier\n", idx);
    retcode = pthread_barrier_wait(barrier_ptr);
    if (retcode != PTHREAD_BARRIER_SERIAL_THREAD) {
      handle_thread_error(retcode, "pthread_barrier_wait", THREAD_EXIT);
    }
    time_t t1 = time(NULL);
    printf("idx=%d tid=%ld waited for barrier step=%d t=%ld\n", idx, (long) pthread_self(), (int) step, (long)(t1-t0));

    if (idx % (2*step) == 0) {
      int other_idx = idx + step;
      // printf("idx=%d other_idx=%d step=%u thread_count=%u\n", idx, other_idx, step, thread_count);
      if (other_idx < thread_count) {
        size_t i = 0;
        size_t j = 0;
        size_t k = 0;
        size_t m = segments[idx].nmemb;
        size_t n = segments[other_idx].nmemb;
        size_t total_len = m + n;
        void *merge_data = malloc(total_len * size);
        void *target = merge_data;
        void *left = segments[idx].base;
        void *right = segments[other_idx].base;
        // printf("idx=%d other_idx=%d step=%u m=%ld n=%ld total_len=%ld\n", idx, other_idx, step, m, n, total_len);

        while (i+j < total_len) {
          if (i >= m) {
            size_t moved_size = size * (n-j);
            memcpy(target, right, moved_size);
            j = n;
            target += moved_size;
            k += n-j;
          } else if (j >= n) {
            size_t moved_size = size * (m-i);
            memcpy(target, left, moved_size);
            i = m;
            target += moved_size;
            k += m-i;
          } else {
            if (compare(left, right, compare_data) <= 0) {
              memcpy(target, left, size);
              target += size;
              left += size;
              i++;
              k++;
            } else {
              memcpy(target, right, size);
              target += size;
              right += size;
              j++;
              k++;
            }
          }
        }
        segments[idx].nmemb = total_len;
        memcpy(segments[idx].base, merge_data, total_len * size);
        free(merge_data);
        segments[other_idx].nmemb = 0;
        segments[other_idx].base = NULL;
      }
    }
    t1 = time(NULL);
    printf("idx=%d tid=%ld merged step=%d t=%ld\n", idx, (long) pthread_self(), (int) step, (long)(t1-t0));
  }
  t1 = time(NULL);
  printf("idx=%d tid=%ld done t=%ld\n", idx, (long) pthread_self(), (long)(t1-t0));
  return (void *) NULL;
}

void psort_r(void *base,
             size_t nmemb,
             size_t size,
             compare_fun3 compare,
             void *argc,
             metric_fun2 metric,
             void *argm,
             unsigned int thread_count,
             enum sort_type selected_sort_type) {

  int retcode;

  if (nmemb <= 1) {
    return;
  }
  assert(thread_count > 0);

  pthread_t                *thread      = (pthread_t *)                malloc(thread_count * sizeof(pthread_t));
  struct sortable_data     *segments    = (struct sortable_data *)     malloc(thread_count * sizeof(struct sortable_data));
  struct psort_thread_data *thread_data = (struct psort_thread_data *) malloc(thread_count * sizeof(struct psort_thread_data));

  pthread_barrier_t barrier;
  retcode = pthread_barrier_init(&barrier, NULL, thread_count);
  handle_thread_error(retcode, "pthread_barrier_init", PROCESS_EXIT);

  unsigned int rest_count = thread_count;
  size_t       rest_len   = nmemb;
  void *ptr = base;
  // printf("thread_count=%u nmemb=%lu partion=[", thread_count, nmemb);
  for (unsigned int i = 0; i < thread_count; i++) {
    thread_data[i].thread_count       = thread_count;
    thread_data[i].thread_idx         = i;
    thread_data[i].segments           = segments;
    thread_data[i].barrier_ptr        = &barrier;
    thread_data[i].selected_sort_type = selected_sort_type;
    thread_data[i].compare            = compare;
    thread_data[i].compare_data       = argc;
    thread_data[i].metric             = metric;
    thread_data[i].metric_data        = argm;

    size_t len_per_thread = rest_len / rest_count;
    segments[i].base = ptr;
    segments[i].size = size;
    segments[i].nmemb = len_per_thread;
    // printf(" %ld ", len_per_thread);
    if (i == thread_count-1) {
      assert(rest_len == len_per_thread);
    }
    ptr += size * len_per_thread;
    rest_count--;
    rest_len -= len_per_thread;
  }
  // printf("]\n");

  for (unsigned int i = 0; i < thread_count; i++) {
    // printf("idx=%d=%d base=%p nmemb=%lu size=%lu\n", i, thread_data[i].thread_idx, thread_data[i].segments[i].base, thread_data[i].segments[i].nmemb, thread_data[i].segments[i].size);
    retcode = pthread_create((thread+i), NULL, psort_thread_run, (thread_data+i));
    handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);
  }
  // printf("threads created\n");

  for (unsigned int i = 0; i < thread_count; i++) {
    /* printf("joining thread %d\n", i); */
    retcode = pthread_join(thread[i], NULL);
    handle_thread_error(retcode, "pthread_join", PROCESS_EXIT);
    /* printf("main: joined thread %d\n", i); */
  }
  // printf("threads joined\n");

  pthread_barrier_destroy(&barrier);

  void *data = segments[0].base;
  if (data != base) {
    memcpy(base, data, nmemb*size);
    // free(segments[0].base); TODO memory-leak!!!
  }
  free(segments);
  free(thread_data);
  free(thread);
}


void parallel_hsort_r(void *base,
                      size_t nmemb,
                      size_t size,
                      compare_fun3 compare,
                      void *argc,
                      unsigned int thread_count) {

  psort_r(base,
          nmemb,
          size,
          compare,
          argc,
          NULL,
          NULL,
          thread_count,
          HEAP_SORT);
}

void parallel_ternary_hsort_r(void *base,
                              size_t nmemb,
                              size_t size,
                              compare_fun3 compare,
                              void *argc,
                              unsigned int thread_count) {

  psort_r(base,
          nmemb,
          size,
          compare,
          argc,
          NULL,
          NULL,
          thread_count,
          TERNARY_HEAP_SORT);
}

void parallel_qsort_r(void *base,
                      size_t nmemb,
                      size_t size,
                      compare_fun3 compare,
                      void *argc,
                      unsigned int thread_count) {

  psort_r(base,
          nmemb,
          size,
          compare,
          argc,
          NULL,
          NULL,
          thread_count,
          QUICK_SORT);
}

void parallel_isort_r(void *base,
                      size_t nmemb,
                      size_t size,
                      compare_fun3 compare,
                      void *argc,
                      unsigned int thread_count) {

  psort_r(base,
          nmemb,
          size,
          compare,
          argc,
          NULL,
          NULL,
          thread_count,
          INSERTION_SORT);
}

void parallel_msort_r(void *base,
                      size_t nmemb,
                      size_t size,
                      compare_fun3 compare,
                      void *argc,
                      unsigned int thread_count) {

  psort_r(base,
          nmemb,
          size,
          compare,
          argc,
          NULL,
          NULL,
          thread_count,
          MERGE_SORT);
}

void parallel_fsort_r(void *base,
                      size_t nmemb,
                      size_t size,
                      compare_fun3 compare,
                      void *argc,
                      metric_fun2 metric,
                      void *argm,
                      unsigned int thread_count) {

  psort_r(base,
          nmemb,
          size,
          compare,
          argc,
          metric,
          argm,
          thread_count,
          FLASH_SORT);
}
