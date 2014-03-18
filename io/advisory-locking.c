/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <sys/file.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include <itskylib.h>

int main(int argc, char *argv[]) {
  const char *FILENAME = "/tmp/abc987654";
  int return_code;
  int myerrno;

  FILE *file = fopen(FILENAME, "a");
  handle_ptr_error(file, "fopen", PROCESS_EXIT);

  fwrite("abcdefghijklmnopqrstuvwxyz", 1, 26, file);
  fwrite("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 1, 26, file);
  int fd = fileno(file);
  int pid = getpid();
  /* try non-blocking-lock */
  lseek(fd, 0, SEEK_SET);
  int pos = 0;
  while (1) {
    return_code = lockf(fd, F_TLOCK, 1);
    if (return_code < 0) {
      myerrno = errno;
      if (myerrno == EACCES || myerrno == EAGAIN) {
        printf("pid=%d could not lock position %d\n", pid, pos);
      } else {
        handle_error_myerrno(return_code, myerrno, "lockf", PROCESS_EXIT);
      }
    } else {
      printf("pid=%d could lock position %d\n", pid, pos);
      break;
    }
    pos++;
    lseek(fd, pos, SEEK_SET);
  }
  sleep(5);
  lockf(fd, F_ULOCK, 1);
  printf("pid=%d unlocked position %d\n", pid, pos);
  close(fd);
  exit(0);
}
