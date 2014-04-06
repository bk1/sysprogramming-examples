/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <itskylib.h>

#define BUF_SIZE 16384

#define ALPHA_SIZE 256

#define SEM_NAME "/sem-thread-memory-count-posix-semaphore"
#define PERM 0600

struct data {
  long counter[ALPHA_SIZE];
};

const int SIZE = sizeof(struct data);

static sem_t *sem_arr[ALPHA_SIZE];
static sem_t *output_sem;

static struct data data;

/* thread function */
void *run(void *raw_name) {
  int retcode;
  time_t thread_start = time(NULL);
  char *name = (char *) raw_name;
  int fd = -1;
  time_t open_start = time(NULL);
  while (fd == -1) {
    fd = open(raw_name, O_RDONLY);
    if (fd < 0 && errno == EMFILE) {
      sleep(1);
      continue;
    }
    if (fd < 0) {
      char msg[256];
      sprintf(msg, "error while opening file=%s", name);
      handle_error(fd, msg, THREAD_EXIT);
    }
  }
  time_t open_duration = time(NULL) - open_start;
  time_t total_sem_wait = 0;
  char buffer[BUF_SIZE];
  while (TRUE) {
    ssize_t size_read = read(fd, buffer, BUF_SIZE);
    if (size_read == 0) {
      /* end of file */
      break;
    }
    if (size_read < 0) {
      /* file cannot be read, just ignore it */
      if (errno == 9) {
        close(fd);
        pthread_exit(NULL);
      }
      char msg[256];
      sprintf(msg, "error while reading file=%s", name);
      handle_error(size_read, msg, THREAD_EXIT);
    }
    int i;
    for (i = 0; i < size_read; i++) {
      unsigned char c = buffer[i];
      time_t t0 = time(NULL);
      retcode = sem_wait(sem_arr[c]);
      time_t dt = time(NULL) - t0;
      total_sem_wait += dt;
      handle_error(retcode, "error while getting sem", THREAD_EXIT);
      long *counter = data.counter;
      counter[c]++;
      retcode = sem_post(sem_arr[c]);
      handle_error(retcode, "error while releasing sem", THREAD_EXIT);
    }
  }
  close(fd);

  time_t thread_duration = time(NULL) - thread_start;

  unsigned int i;

  sem_wait(output_sem);
  printf("------------------------------------------------------------\n");
  printf("%s: pid=%ld\n", name, (long) getpid());
  printf("open duration: ~ %ld sec; total wait for data: ~ %ld sec; thread duration: ~ %ld\n", (long) open_duration, (long) total_sem_wait, (long) thread_duration);
  printf("------------------------------------------------------------\n");
  for (i = 0; i < ALPHA_SIZE; i++) {
    long *counter = data.counter;

    retcode = sem_wait(sem_arr[i]);
    handle_error(retcode, "error while getting sem", THREAD_EXIT);

    long val = counter[i];

    retcode = sem_post(sem_arr[i]);
    handle_error(retcode, "error while releasing sem", THREAD_EXIT);

    if (! (i & 007)) {
      printf("\n");
    }
    if ((i & 0177) < 32 || i == 127) {
      printf("\\%03o: %10ld    ", i, val);
    } else {
      printf("%4c: %10ld    ", (char) i, val);
    }
  }
  printf("\n\n");
  printf("------------------------------------------------------------\n\n");
  fflush(stdout);
  sem_post(output_sem);
  return NULL;
}


int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("Usage\n\n");
    printf("%s file1 file2 file3 ... filen\ncount files, show accumulated output after having completed one file\n\n", argv[0]);
    exit(1);
  }

  time_t start_time = time(NULL);
  int retcode = 0;
  int i;

  printf("%d files will be read\n", argc-1);
  fflush(stdout);

  for (i = 0; i < ALPHA_SIZE; i++) {
    data.counter[i] = 0L;
  }

  char raw_name[50];
  char *names[ALPHA_SIZE];
  char *output_name;
  for (i = 0; i < ALPHA_SIZE; i++) {
    sprintf(raw_name, "%s%03d", SEM_NAME, i);
    char *name = strdup(raw_name);
    names[i] = name;
    sem_t *sem_ptr = sem_open(name, O_CREAT, PERM, 1);
    handle_ptr_error(sem_ptr, "creating sem", PROCESS_EXIT);
    sem_arr[i] = sem_ptr;
  }
  sprintf(raw_name, "%s-output", SEM_NAME);
  char *name = strdup(raw_name);
  output_name = name;
  sem_t *sem_ptr = sem_open(name, O_CREAT, PERM, 1);
  output_sem = sem_ptr;
  handle_error(retcode, "creating sem", PROCESS_EXIT);

  pthread_t *threads = malloc((argc-1)*sizeof(pthread_t));
  for (i = 1; i < argc; i++) {
    retcode = pthread_create(&(threads[i-1]), NULL, run, argv[i]);
    handle_error(retcode, "starting thread", PROCESS_EXIT);
  }
  sem_wait(output_sem);
  printf("%d threads started\n", argc-1);
  fflush(stdout);
  sem_post(output_sem);
  for (i = 0; i < argc-1; i++) {
    retcode = pthread_join(threads[i], NULL);
    handle_error(retcode, "joining thread", PROCESS_EXIT);
  }
  for (i = 0; i < ALPHA_SIZE; i++) {
    retcode = sem_close(sem_arr[i]);
    handle_error(retcode, "closing sem", PROCESS_EXIT);
    retcode = sem_unlink(names[i]);
    handle_error(retcode, "unlinking sem", PROCESS_EXIT);
  }
  retcode = sem_close(output_sem);
  handle_error(retcode, "closing sem", PROCESS_EXIT);
  retcode = sem_unlink(output_name);
  handle_error(retcode, "unlinking sem", PROCESS_EXIT);

  time_t total_time = time(NULL) - start_time;
  printf("total %ld sec\n", (long) total_time);
  printf("done\n");
  exit(0);
}
