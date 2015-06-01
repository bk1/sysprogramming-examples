/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-06-02
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/*Include necessary header files.*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <itskylib.h>
#include <pthread.h>

#include <itskylib.h>

int buffer_size;

pthread_barrier_t start_barrier;

void usage(char *msg, char *argv0) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("USAGE:\n\n%s buffersize inpipe1 inpipe2 ... inpipen\n", argv0);
  if (msg != NULL && strlen(msg) > 0) {
    exit(1);
  } else {
    exit(0);
  }
}

void *handle_pipe(void *targ) {
  char *name = (char *) targ;
  int retcode;
  // printf("starting thread pipe name=%s\n", name);
  retcode = pthread_barrier_wait(& start_barrier);
  if (retcode < 0 && retcode != PTHREAD_BARRIER_SERIAL_THREAD) {
    handle_thread_error(retcode, "pthread_barrier_wait", THREAD_EXIT);
  }
  // printf("opening pipe name=%s\n", name);
  int fd = open(name, O_RDONLY);
  // printf("opened fd=%d for pipe name=%s\n", fd, name);
  handle_error(fd, "open", THREAD_EXIT);
  char *buffer = (char *) malloc((buffer_size + 1) * sizeof(char));
  // printf("starting thread on fd=%d\n", fd);
  int n = read(fd, buffer, buffer_size);
  handle_error(n, "read", THREAD_EXIT);
  buffer[n] = '\000';
  printf("for pipe name=%s (fd=%d) found data:\n\"%s\"\n", name, fd, buffer);
  sleep(1);
  printf("exiting\n");
  exit(0);
  return NULL;
}

int main(int argc, char **argv) {
  int retcode;
  if (argc == 2 && strcmp(argv[1], "--help") == 0) {
    usage(NULL, argv[0]);
  }
  if (argc < 3) {
    usage("not enough arguments", argv[0]);
  }

  buffer_size = atoi(argv[1]);
  if (buffer_size <= 0) {
    usage("buffersize must be > 0", argv[0]);
  }
  int nfds = argc-2;
  // printf("nfds=%d\n", nfds);

  retcode = pthread_barrier_init(&start_barrier, NULL, nfds);
  handle_thread_error(retcode, "pthread_barrier_init", PROCESS_EXIT);

  for (int i = 0; i < nfds; i++) {
    char *name = argv[2+i];
    // printf("creating thread for pipe name=%s\n", name);
    pthread_t thread;
    retcode = pthread_create(&thread, NULL, handle_pipe, name);
    handle_thread_error(retcode, "pthread_create", PROCESS_EXIT);
  }
  /* let the child threads do the work */
  // printf("exiting main thread\n");
  pthread_exit(NULL);
  exit(0);
}
