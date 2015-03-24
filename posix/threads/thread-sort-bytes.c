/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-23
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* enable qsort_r */
#define _GNU_SOURCE

#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <itskylib.h>
#include <hsort.h>
#include <fsort.h>
#include <isort.h>
#include <ternary-hsort.h>
#include <msort.h>
#include <fsort-metrics.h>
#include <psort.h>

int thread_count;

int data_size;
char *data;

struct thread_arg {
  int thread_idx;
  pthread_barrier_t *barrier;
  char *data;
  size_t size;
  struct thread_arg *arr;
};

void *thread_run(void *ptr);
void prepare(const char * file_name);

void usage(char *argv0, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n");

  printf("splits the given input file into bytes and sorts them using n threads\n");
  printf("\n");
  printf("%s n file\n", argv0);
  exit(1);
}

void print_data() {
  sleep(1);
  printf("------------------------------------------------------------\n");
  int w = 0;
  for (int i = 0; i < data_size; i++) {
    int x = data[i];
    if (x <32) {
      printf("\\%03o", x);
      w+=4;
    } else {
      printf("%c", x);
      w++;
    }
    if (w > 180) {
      printf("\n");
      w = 0;
    }
  }
  printf("\n");
  printf("------------------------------------------------------------\n");
}

int main(int argc, char *argv[]) {
  int retcode;
  pthread_t *thread;

  char *argv0 = argv[0];
  if (argc != 3) {
    printf("found %d arguments\n", argc - 1);
    usage(argv0, "wrong number of arguments");
  }

  thread_count = atoi(argv[1]);
  const char *file_name = argv[2];

  prepare(file_name);

  thread = (pthread_t *) malloc(thread_count * sizeof(pthread_t));
  struct thread_arg *thread_data = (struct thread_arg *) malloc(thread_count * sizeof(struct thread_arg));

  pthread_barrier_t barrier;
  retcode = pthread_barrier_init(&barrier, NULL, thread_count + 1);

  size_t size_per_thread = data_size / thread_count;
  size_t rest = data_size;
  char *partial_data = data;

  for (int i = 0; i < thread_count; i++) {
    thread_data[i].thread_idx = i;
    thread_data[i].barrier = &barrier;
    thread_data[i].data = partial_data;
    thread_data[i].size = i == thread_count - 1 ? rest : size_per_thread;
    thread_data[i].arr = thread_data;

    rest -= size_per_thread;
    partial_data += size_per_thread;
    retcode = pthread_create(&(thread[i]), NULL, thread_run, &(thread_data[i]));
    handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);
  }
  for (int step = 1; step < thread_count; step *= 2) {
    // printf("waiting for barrier in main\n");
    retcode =   pthread_barrier_wait(&barrier);
    // printf("waited for barrier in main (step=%d)\n", step);
    if (retcode != PTHREAD_BARRIER_SERIAL_THREAD && retcode != 0) {
      printf("waited for barrier in main (step=%d)\n", step);
      handle_thread_error(retcode, "pthread_barrier_wait", THREAD_EXIT);
    }
    // printf("sorting of partitions finished for step=%d\n", step);
  }
  for (int i = 0; i < thread_count; i++) {
    /* printf("main: joining thread %d\n", i); */
    retcode = pthread_join(thread[i], NULL);
    handle_thread_error(retcode, "pthread_join", PROCESS_EXIT);
    /* printf("main: joined thread %d\n", i); */
  }
  print_data();
  pthread_barrier_destroy(&barrier);
  exit(0);
}

void prepare(const char * file_name) {
  // int retcode;
  printf("opening %s\n", file_name);
  int fd = open(file_name, O_RDONLY);
  handle_error(fd, "open", PROCESS_EXIT);
  struct stat stat;
  int retcode = fstat(fd, &stat);
  handle_error(retcode, "open", PROCESS_EXIT);
  off_t size = stat.st_size;
  if (size <= 0) {
    handle_error(-1, "size not positive", PROCESS_EXIT);
    exit(1);
  }
  char *buffer = (char *) malloc(size);
  retcode = read(fd, buffer, size);
  handle_error(retcode, "read", PROCESS_EXIT);
  retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);
  data = buffer;
  data_size = size;
}

int compare(const void *x_ptr, const void *y_ptr) {
  char x = *((char *) x_ptr);
  char y = *((char *) y_ptr);
  return x-y;
}

void *thread_run(void *ptr) {
  int retcode;
  struct thread_arg *arg = (struct thread_arg *) ptr;
  qsort(arg->data, arg->size, 1, compare);
  int idx = arg->thread_idx;
  struct thread_arg *arr = arg->arr;
  for (int step = 1; step < thread_count; step *= 2) {
    retcode = pthread_barrier_wait(arg->barrier);
    if (retcode != PTHREAD_BARRIER_SERIAL_THREAD && retcode != 0) {
      printf("waited for barrier in child %d (step=%d retcode=%d)\n", idx, step, retcode);
      handle_thread_error(retcode, "pthread_barrier_wait", THREAD_EXIT);
    }

    if (idx % (2*step) == 0) {
      int other_idx = idx + step;
      if (other_idx < thread_count) {
        int m = arr[idx].size;
        int n = arr[other_idx].size;
        int i = 0;
        int j = 0;
        int k = 0;
        int total_len = m + n;
        char *copy = malloc(total_len);
        char *left = arr[idx].data;
        char *right = arr[other_idx].data;
        while (i + j < total_len) {
          if (i >= m) {
            memcpy(copy + k, right + j, n - j);
            j = n;
          } else if (j >= n) {
            memcpy(copy + k, left + i, m - i);
            i = m;
          } else {
            if (left[i] <= right[j]) {
              copy[k++] = left[i++];
            } else {
              copy[k++] = right[j++];
            }
          }
        }
        arr[idx].size = total_len;
        memcpy(left, copy, total_len);
        arr[other_idx].size = 0;
        arr[other_idx].data = NULL;
        free(copy);
      }
    }
  }
  return (void *) NULL;
}
