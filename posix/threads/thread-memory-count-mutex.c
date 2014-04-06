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

#define FALSE 0
#define TRUE  1

#define ERROR_SIZE 16384

#define BUF_SIZE 16384

#define ALPHA_SIZE 256
#define SEM_SIZE 128

struct data {
  long counter[ALPHA_SIZE];
};

const int SIZE = sizeof(struct data);

static pthread_mutex_t mutex_arr[ALPHA_SIZE];
static pthread_mutex_t output_mutex;

static struct data data;

/* helper function for dealing with errors */
void handle_error(long return_code, const char *msg, int in_thread) {
  if (return_code < 0) {
    char extra_txt[ERROR_SIZE];
    char error_msg[ERROR_SIZE];
    char *extra_msg = extra_txt;
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    if (msg != NULL) {
      sprintf(extra_msg, "%s\n", msg);
    } else {
      extra_msg = "";
    }
    sprintf(error_msg, "%sreturn_code=%ld\nerrno=%d\nmessage=%s\n", extra_msg, return_code, myerrno, error_str);
    write(STDOUT_FILENO, error_msg, strlen(error_msg));
    /* cleanup(); */
    if (in_thread) {
      pthread_exit(NULL);
    } else {
      exit(1);
    }
  }
}

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
      handle_error(fd, msg, TRUE);
    }
  }
  time_t open_duration = time(NULL) - open_start;
  time_t total_mutex_wait = 0;
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
      handle_error(size_read, msg, TRUE);
    }
    int i;
    for (i = 0; i < size_read; i++) {
      unsigned char c = buffer[i];
      time_t t0 = time(NULL);
      retcode = pthread_mutex_lock(&(mutex_arr[c]));
      time_t dt = time(NULL) - t0;
      total_mutex_wait += dt;
      handle_error(retcode, "error while getting mutex", TRUE);
      long *counter = data.counter;
      counter[c]++;
      retcode = pthread_mutex_unlock(& (mutex_arr[c]));
      handle_error(retcode, "error while releasing mutex", TRUE);
    }
  }
  close(fd);

  time_t thread_duration = time(NULL) - thread_start;

  unsigned int i;

  pthread_mutex_lock(&output_mutex);
  printf("------------------------------------------------------------\n");
  printf("%s: pid=%ld\n", name, (long) getpid());
  printf("open duration: ~ %ld sec; total wait for data: ~ %ld sec; thread duration: ~ %ld\n", (long) open_duration, (long) total_mutex_wait, (long) thread_duration);
  printf("------------------------------------------------------------\n");
  for (i = 0; i < ALPHA_SIZE; i++) {
    long *counter = data.counter;

    retcode = pthread_mutex_lock(&(mutex_arr[i]));
    handle_error(retcode, "error while getting mutex", TRUE);

    long val = counter[i];

    retcode = pthread_mutex_unlock(& (mutex_arr[i]));
    handle_error(retcode, "error while releasing mutex", TRUE);

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
  pthread_mutex_unlock(&output_mutex);
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

  for (i = 0; i < ALPHA_SIZE; i++) {
    retcode = pthread_mutex_init(&(mutex_arr[i]), NULL);
    handle_error(retcode, "creating mutex", FALSE);
  }
  retcode = pthread_mutex_init(&output_mutex, NULL);
  handle_error(retcode, "creating mutex", FALSE);

  pthread_t *threads = malloc((argc-1)*sizeof(pthread_t));
  for (i = 1; i < argc; i++) {
    retcode = pthread_create(&(threads[i-1]), NULL, run, argv[i]);
    handle_error(retcode, "starting thread", FALSE);
  }
  pthread_mutex_lock(&output_mutex);
  printf("%d threads started\n", argc-1);
  fflush(stdout);
  pthread_mutex_unlock(&output_mutex);
  for (i = 0; i < argc-1; i++) {
    retcode = pthread_join(threads[i], NULL);
    handle_error(retcode, "joining thread", FALSE);
  }
  for (i = 0; i < ALPHA_SIZE; i++) {
    retcode = pthread_mutex_destroy(&(mutex_arr[i]));
    handle_error(retcode, "destroying mutex", FALSE);
  }
  retcode = pthread_mutex_destroy(&output_mutex);
  handle_error(retcode, "destroying mutex", FALSE);

  time_t total_time = time(NULL) - start_time;
  printf("total %ld sec\n", (long) total_time);
  printf("done\n");
  exit(0);
}
