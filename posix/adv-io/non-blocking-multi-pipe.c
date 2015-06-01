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

int main(int argc, char **argv) {
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

  int *fds = (int *) malloc(sizeof(int) * nfds);
  handle_ptr_error(fds, "malloc", PROCESS_EXIT);

  char *buffer = (char *) malloc((buffer_size + 1) * sizeof(char));
  handle_ptr_error(buffer, "malloc", PROCESS_EXIT);
  
  for (int i = 0; i < nfds; i++) {
    char *name = argv[2+i];
    int fd = open(name, O_RDONLY | O_NONBLOCK);
    handle_error(fd, "open", PROCESS_EXIT);
    fds[i] = fd;
  }
  int done = FALSE;
  int idx = -1;
  int pos = -1;
  while (! done) {
    for (int i = 0; i < nfds; i++) {
      int n = read(fds[i], buffer, buffer_size);
      if (n > 0) {
        done = TRUE;
        idx = i;
        pos = n;
        break;
      }
      if (errno == EAGAIN) {
        continue;
      }
      handle_error(n, "read", PROCESS_EXIT);
    }
    sleep(1);
  }
  buffer[pos] = '\000';
  printf("for pipe %d name=%s (fd=%d) found data:\n\"%s\"\n", idx, argv[idx + 1], fds[idx], buffer);
  sleep(1);
  printf("exiting\n");
  exit(0);
}
