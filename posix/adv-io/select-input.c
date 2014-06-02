
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

int main(int argc, char **argv) {
  int retcode;
  if (argc < 3) {
    printf("USAGE:\n\n%s buffersize inpipe1 inpipe2 ... inpipen\n", argv[0]);
    exit(1);
  }

  int buffer_size = atoi(argv[1]);
  char *buffer = malloc(buffer_size + 1);

  int nfds = argc-2;

  int fdin[nfds];
  fd_set fdin_set;
  FD_ZERO(&fdin_set);
  for (int i = 0; i < nfds; i++) {
    printf("opening fdin[%d] for %s\n", i, argv[2+i]);
    fdin[i] = open(argv[2+i], O_RDONLY | O_NONBLOCK);
    handle_error(fdin[i], "open", PROCESS_EXIT);
    FD_SET(fdin[i], &fdin_set);
  }

  struct timespec timeout;
  timeout.tv_sec = 10;
  timeout.tv_nsec = 0;
  retcode = select(nfds, &fdin_set, NULL, NULL, &timeout);
  handle_error(retcode, "select", PROCESS_EXIT);
  printf("found entries for %d descriptors", retcode);
  for (int i = 0; i < nfds; i++) {
    if (FD_ISSET(fdin[i], &fdin_set)) {
      printf("found stuff for i=%d fd=%d\n", i, fdin[i]);
      int n = read(fdin[i], buffer, buffer_size);
      buffer[n] = (char) 0;
      printf("found %d chars: \"%s\"\n", n, buffer);
    }
  }
  exit(0);
}
