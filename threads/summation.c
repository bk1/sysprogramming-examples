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
#include <stdbool.h>

#include <itskylib.h>

#define SIZE 1024

struct thread_arg {
  const char *file_name;
  long *partial_sums;
  long *total_sum;
  int thread_idx;
  int thread_count;
  pthread_barrier_t *barrier;
  pthread_cond_t  *condition_1;
};




void *thread_run(void *ptr) {
  int retcode;
  struct thread_arg *arg = (struct thread_arg *) ptr;
  const char *file_name = arg->file_name;
  int fd = open(file_name, O_RDONLY);
  int idx = arg->thread_idx;
  (arg->partial_sums)[idx] = 0L;
  unsigned char buff[SIZE];
  while (TRUE) {
    size_t count = read(fd, buff, SIZE);
    if (count <= 0) {
      break;
    }
    for (int i = 0; i < count; i++) {
      unsigned char c = buff[i];
      (arg->partial_sums)[idx] += c;
    }
  }

  retcode = pthread_barrier_wait(arg->barrier);
  if (retcode == PTHREAD_BARRIER_SERIAL_THREAD) {
    (arg->total_sum)[0] = 0L;
    for (int i = 0; i < arg->thread_count; i++) {
      (arg->total_sum)[0] += (long) (arg->partial_sums)[i];
      // printf("%d : %d : %ld (%ld)\n", idx, i, *(arg->total_sum), (arg->partial_sums)[i]);
    }
    retcode = pthread_cond_signal(arg->condition_1);
  } else {
    handle_thread_error(retcode, "pthread_barrier_wait", THREAD_EXIT);
  }
  sleep(10);
  return (void *) NULL;
}

void usage(char *argv0, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n");
  printf("%s file1 file2 file3 ... filen\n\tadds all characters of all files, one file per thread and aggregates to one sum in the end.\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {
  int retcode;
  pthread_t *thread;

  char *argv0 = argv[0];
  if (is_help_requested(argc, argv)) {
    usage(argv0, "");
  }

  int thread_count = argc - 1;
  
  thread = (pthread_t *) malloc(thread_count * sizeof(pthread_t));

  pthread_barrier_t barrier;
  pthread_cond_t condition_1;
  pthread_mutex_t mutex;
  retcode = pthread_barrier_init(&barrier, NULL, thread_count);
  handle_thread_error(retcode, "pthread_barrier_init", PROCESS_EXIT);
  retcode = pthread_cond_init(&condition_1, NULL);
  handle_thread_error(retcode, "pthread_cond_init (1)", PROCESS_EXIT);
  retcode = pthread_mutex_init(&mutex, NULL);
  handle_thread_error(retcode, "pthread_mutex_init (1)", PROCESS_EXIT);
  
  long total_sum;
  long partial_sums[thread_count];
  struct thread_arg args[thread_count];
  for (int i = 0; i < thread_count; i++) {
    args[i].thread_idx = i;
    args[i].file_name = argv[i+1];
    args[i].total_sum = &total_sum;
    args[i].partial_sums = partial_sums;
    args[i].barrier = &barrier;
    args[i].condition_1 = &condition_1;
    args[i].thread_count = thread_count;
  }

  for (int i = 0; i < thread_count; i++) {
    retcode = pthread_create(&(thread[i]), NULL, thread_run, &(args[i]));
    handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);
  }

  retcode = pthread_cond_wait(&condition_1, &mutex);
  handle_thread_error(retcode, "pthread_cond_wait", PROCESS_EXIT);

  // total_sum = 0L;
  for (int i = 0; i < thread_count; i++) {
    printf("%s : %ld\n", argv[i+1], partial_sums[i]);
    // total_sum += partial_sums[i];
  }
  printf("\n");
  printf("total: %ld\n", total_sum);
  for (int i = 0; i < thread_count; i++) {
    retcode = pthread_join(thread[i], NULL);
    handle_thread_error(retcode, "pthread_join", PROCESS_EXIT);
  }


  pthread_barrier_destroy(&barrier);
  exit(0);
}
