/* (C) IT Sky Consulting GmbH / Steve Heller 2014
 * http://www.it-sky-consulting.com/
 * Author: Steve Heller
 * Date: 2014-06-02
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>


#include <itskylib.h>

const int SIZE = 256;
const int SIZE_PLUS = 256 + 1;

enum blocking { BLOCKING, NON_BLOCKING };

int read_mode = 0;
int open_mode = 0;

void usage(char *msg, char *argv0) {
  printf("%s\n\n", msg);
  printf("usage:\n\n");
  printf("%s -rb -ob for blocking read (open blocking)\n", argv0);
  printf("%s -rb -on for blocking read (open nonblocking)\n", argv0);
  printf("%s -rn -ob for non-blocking read (open blocking)\n", argv0);
  printf("%s -rn -on for non-blocking read (open nonblocking)\n", argv0);
  exit(1);
}

void do_blocking(int fd) {
  int retcode = fcntl(fd, F_SETFL, fcntl(0, F_GETFL) & ~ O_NONBLOCK);
  handle_error(retcode, "fctnl F_SETFL O_NONBLOCK", PROCESS_EXIT);
  while(TRUE) {
    char input_string[SIZE_PLUS];
    int retcode = read(fd, input_string, SIZE);
    if (retcode == 0) {
      break;
    }
    handle_error(retcode, "read blocking", PROCESS_EXIT);
    input_string[retcode] = '\000';
    printf("You entered: %s\n",input_string);
  }
}

void do_non_blocking(int fd) {
  int retcode = fcntl(fd, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
  handle_error(retcode, "fctnl F_SETFL O_NONBLOCK", PROCESS_EXIT);
  while(TRUE) {
    char input_string[SIZE_PLUS];
    memset(input_string, 0, SIZE_PLUS);
    int retcode = read(fd, input_string, SIZE);
    if (retcode == 0) {
      break;
    } else if (retcode < 0 && errno == EAGAIN) {
      printf("You entered nothing!\n");
      sleep(1);
      continue;
    }
    handle_error(retcode, "read blocking", PROCESS_EXIT);
    input_string[retcode] = '\000';
    printf("You entered: %s\n",input_string);
    sleep(1);
  }
}

void do_stuff() {
  int flags;
  if (open_mode == BLOCKING) {
    flags = O_RDONLY;
  } else if (open_mode == NON_BLOCKING) {
    flags = O_RDONLY | O_NONBLOCK;
  } else {
    usage("internal error: WRONG USAGE, TRY AGAIN\n", "-");
  }
  int fd = open("/dev/tty", flags);
  
  if (read_mode == BLOCKING) { // blocking
    do_blocking(fd);
  } else if (read_mode == NON_BLOCKING) { // non-blocking
    do_non_blocking(fd);
  } else {
    usage("internal error: WRONG USAGE, TRY AGAIN\n", "-");
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    usage("wrong number of arguments", argv[0]);
  }
  if (strcmp(argv[1], "-rb") == 0) {
    read_mode = BLOCKING;
  } else if (strcmp(argv[1], "-rn") == 0) {
    read_mode = NON_BLOCKING;
  } else {
    usage("wrong first argument (-rb and -rn supported)", argv[0]);
  }
  if (strcmp(argv[2], "-ob") == 0) {
    open_mode = BLOCKING;
  } else if (strcmp(argv[2], "-on") == 0) {
    open_mode = NON_BLOCKING;
  } else {
    usage("wrong seconde argument (-ob and -on supported)", argv[0]);
  }
  do_stuff();
  exit(0);
}
