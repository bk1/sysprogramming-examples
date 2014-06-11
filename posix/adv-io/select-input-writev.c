/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <itskylib.h>
#include <sys/select.h>
#include <sys/uio.h>

int main(int argc, char **argv) {
  int retcode;
  if (argc < 5) {
    printf("USAGE:\n\n%s buffersize sec usec inpipe1 inpipe2 ... inpipen\n", argv[0]);
    exit(1);
  }

  int buffer_size = atoi(argv[1]);
  int sec = atoi(argv[2]);
  int nsec = atoi(argv[3]);
  int nfds = argc-4;
  char **buffer = malloc(nfds * sizeof(char *));

  int *fdin = (int *) malloc(nfds * sizeof(int));
  fd_set fdin_set;
  FD_ZERO(&fdin_set);
  for (int i = 0; i < nfds; i++) {
    printf("opening fdin[%d] for %s\n", i, argv[4+i]);
    fdin[i] = open(argv[4+i], O_RDONLY | O_NONBLOCK);
    handle_error(fdin[i], "open", PROCESS_EXIT);
    FD_SET(fdin[i], &fdin_set);
    buffer[i] = (char *) malloc(buffer_size);
  }
  if (sec >= 0 && nsec >= 0) {
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = nsec;
    retcode = select(nfds, &fdin_set, NULL, NULL, &timeout);
  } else {
    retcode = select(nfds, &fdin_set, NULL, NULL, NULL);

  }
  handle_error(retcode, "select", PROCESS_EXIT);
  printf("found entries for %d descriptors\n", retcode);
  
  struct iovec *iov = (struct iovec *) malloc(retcode * sizeof(struct iovec));
  int idx = 0;
  for (int i = 0; i < nfds; i++) {
      if (FD_ISSET(fdin[i], &fdin_set)) {
        printf("found stuff for i=%d fd=%d\n", i, fdin[i]);
        int n = read(fdin[i], buffer[i], buffer_size);
        buffer[i][n] = (char) 0;
        printf("found %d chars: \"%s\"\n", n, buffer[i]);
        iov[idx].iov_base = buffer[i];
        iov[idx].iov_len  = n;
        idx++;
      }
  }
  printf("now doing the kewl writev:\n\n");
  fflush(stdout);
  writev(STDOUT_FILENO, iov, retcode);
  exit(0);
}
