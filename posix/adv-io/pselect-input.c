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
#include <sys/select.h>

void usage(char *msg, char *argv0) {
  if (msg != NULL) {
    printf("%s\n\n", msg);
  }
  printf("USAGE:\n\n%s buffersize sec nsec inpipe1 inpipe2 ... inpipen\n", argv0);
  if (msg != NULL) {
    exit(1);
  }
}

int main(int argc, char **argv) {
  if (argc == 2 && strcmp(argv[1], "--help") == 0) {
    usage(NULL, argv[0]);
  }

  if (argc < 5) {
    usage("wrong number of arguments", argv[0]);
  }

  int retcode;

  int buffer_size = atoi(argv[1]);
  char *buffer = malloc(buffer_size + 1);
  int sec = atoi(argv[2]);
  int nsec = atoi(argv[3]);
  int npipes = argc-4;

  int fdin[npipes];
  fd_set fdin_set;
  FD_ZERO(&fdin_set);
  int max_fdin = -1;
  for (int i = 0; i < npipes; i++) {
    printf("opening fdin[%d] for %s\n", i, argv[4+i]);
    fdin[i] = open(argv[4+i], O_RDONLY | O_NONBLOCK);
    handle_error(fdin[i], "open", PROCESS_EXIT);
    FD_SET(fdin[i], &fdin_set);
    max_fdin = fdin[i];
  }
 
  int nfds = max_fdin + 1;

  sigset_t *mask = NULL;
  if (sec >= 0 && nsec >= 0) {
    struct timespec timeout;
    timeout.tv_sec = sec;
    timeout.tv_nsec = nsec;
    retcode = pselect(nfds, &fdin_set, NULL, NULL, &timeout, mask);
  } else {
    retcode = pselect(nfds, &fdin_set, NULL, NULL, NULL, mask);
  }
  handle_error(retcode, "select", PROCESS_EXIT);
  printf("found entries for %d descriptors\n", retcode);
  for (int i = 0; i < npipes; i++) {
    if (FD_ISSET(fdin[i], &fdin_set)) {
      printf("found stuff for i=%d fd=%d\n", i, fdin[i]);
      int n = read(fdin[i], buffer, buffer_size);
      buffer[n] = (char) 0;
      printf("found %d chars: \"%s\"\n", n, buffer);
    }
  }
  exit(0);
}
