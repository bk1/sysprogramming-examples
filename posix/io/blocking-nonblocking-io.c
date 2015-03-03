/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include <itskylib.h>

void usage(char *argv[], char *msg) {
  if (msg != NULL) {
    printf("%s\n\n", msg);
  }
  printf("Usage:\n\n%s -b file\n\n%s -n file\n\nreads file or named pipe in nonblocking or blocking mode and displays content on stdout\n\n", argv[0], argv[0]);
  exit(1);
}

enum io_mode { BLOCKING, NON_BLOCKING };

const size_t BUF_SIZE = 1024;

int main(int argc, char *argv[]) {
 
  if (argc != 3) {
    usage(argv, "wrong number of arguments");
  }

  enum io_mode io_mode;
  
  if (strcmp(argv[1], "-n") == 0) {
    io_mode = NON_BLOCKING;
  } else if (strcmp(argv[1], "-b") == 0) {
    io_mode = BLOCKING;
  } else {
    usage(argv, "only -n and -b as options supported");
  }
  
  int retcode;
  int fd;
  if (io_mode == BLOCKING) {
    fd = open(argv[2], O_RDONLY);
    handle_error(fd, "open blocking", PROCESS_EXIT);
  } else if (io_mode == NON_BLOCKING) {
    fd = open(argv[2], O_RDONLY | O_NONBLOCK);
    handle_error(fd, "open non-blocking", PROCESS_EXIT);
  } else {
    usage(argv, "unsupported io_mode");
  }

  char buf[BUF_SIZE];

  while (TRUE) {
    ssize_t s = read(fd, buf, BUF_SIZE);
    if (s == 0) {
      printf("\nDONE\n");
      break;
    } else if (s == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        printf("\nwaiting for data\n");
        sleep(1);
        continue;
      } else {
        handle_error(s, "read", PROCESS_EXIT);
      }
    }
    while (s > 0) {
      retcode = write(STDOUT_FILENO, buf, s);
      handle_error(retcode, "write", PROCESS_EXIT);
      s -= retcode;
    }
  }
  retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);
  exit(0);
}
